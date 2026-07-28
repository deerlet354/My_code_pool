202604131732 开始例子的编写

																								包括视频总时长，帧间隔时间，
																										/\	
																										|
初始化：注册FFmpeg所有输入和输出设备。 -> 初始化网络模块 -> 分配AVFormatContext（多媒体文件或流的容器上下文） -> 打开视频文件 -> 获取流信息 ->遍历查找音视频流 
																							|
																							\/
																			检测文件格式是否正确，是否为mp4格式，是否有读权限
-> 分配AVCodecContext（视频编解码上下文） -> 根据提供的编解码器参数值填充编解码器上下文(复制编解码器参数) -> 查找解码器 -> 打开解码器
-> 初始化视频部分：AVFrame 帧分配 提前分配好内存和转换器，为后续每一帧的 YUV→RGB 转换做准备。
	-> 原始帧 YUV ， 转换后的RGB帧 -> 计算RGB缓冲区大小 ->   根据指定的图像参数和提供的数组设置数据指针和行大小 -> 转换图像格式 将 YUV 转换为 RGB ->分配AVPacket 	
										|								|								|
										\/								\/					            \/
						av_image_get_buffer_size()		av_image_fill_arrays()					sws_getContext()
-> 创建记录当前帧数据的对象 -> 重写run() ->  启动线程 -> 读包 判断是否为视频流索引 -> 发送包 ->从解码器获取解码后的原始帧数据-> 转换为qt的图片格式->->->->->->->->生产消费模型
	-> 					
						
						
						
202604152025 被老师指出实现过程有问题，以上流程推到重来						
						
----------------------------------------------------------------------------------------------------------------------------------------------


圣经：工程从外向里一步一步剖析

有一个点：错误处理时按分配逆序释放

202604152041 重新开始写例子，工程名 demo_devio		
step1:先运行例子，看一下效果，从开始按键入手 -> ui布局，将button	、label拖入 -> 先拖两个按键：开始和停止 -> 开始写槽函数 -> 我需要这个开始按键的槽函数的作用是打开文件并播放 -> 所以现在要处理视频
step2:创建视频处理的对象 -> 构建基本框架 -> 构造、析构、初始化等 -> 初始化我要知道是否成功，所以初始化函数要有返回值来确定是否初始化成功 -> 因为返回值的不同值代表不同含义，将返回值定义为一个枚举，返回枚举就行
		-> 如果辅助的东西挺多的话，可以创建一个中间头文件，将东西加到这里 -> 先给个成功返回值，在widget的初始化函数里调用视频对象的初始化函数，在mian函数里调用widget的初始化函数。
		-> 返回值没问题继续 -> 初始化注册FFmpeg所有输入和输出设备。 -> 初始化网络模块 -> 从打开文件入手，创建打开文件函数，参数就是文件路径 -> 检查文件格式：后缀、可读、是否为空等
step3:初始化 获取音频与视频信息 -> 音视频编解码上下文及解码器 -> 处理视频 -> 启动线程 -> 需要一个视频标志位来监控视频到哪一步了 -> 重写run 
-> void STAVControl::run(){
    qDebug() << "run enter";
    int iRet = -1;
    int i = 0;
    while(i < 5){
        if(m_iStatus == AV_S_END){
            break;
        }
        iRet = av_read_frame(m_pAVFormatCtx,m_pAVpkt);
        if(iRet == 0){
            if(m_pAVpkt->stream_index == m_VIndex){
                qDebug() << "视频流信息获取成功";
            }
            else{
                qDebug() << "读到的不是视频流";
            }
        }
        else{
            av_packet_unref(m_pAVpkt);
            break;
        }

        qDebug() << "i = " << i;
        i ++;
    }
    qDebug() << "run exit";
}																				  0 	1	  2	    3
像这样调试会发现，i = 3时才会输出 视频流信息获取成功 ，或没循环到第四4时才会输出，因为mp4文件的数据包顺序是：文件头 元数据 音频包 视频包 ...
-> 转换为qt的图片格式：AVFrame -> sws_scale()格式转换 -> RGB内存缓冲区->  QImage -> QPixmap -> QLabel显示

->创建存储单个帧数据的对象 -> 把数据推入Qlist列表 -> 计算下一帧时间 -> 测试放入list里没问题，现在就要显示在label上，通过pushData函数发射信号，在widget里链接信号槽，通过槽函数实现视频播放
-> 在槽函数中要获取数据，所以封装一个获取数据的函数 -> 视频播放需要按照帧显示时间来实现，所以需要QTimer延时，链接信号槽 
-> 使用unsigned int len = tmp->m_iNextTime - tmp->m_iPlayTime;计算帧显示时间时由于第一帧没有next的数据，
	所以在pushdata函数里 计算下一帧时间的语句后加pData->m_iNextTime = pData->m_iPlayTime + m_iIntervalVideoFrame;给个默认next时间
	在OnstartPlay函数里的GatData值调用一次，用于显示第一帧的画面。 持续调用得在time的槽函数里实现 -> 记得在析构函数里写线程安全退出的逻辑
->  m_iStatus = AV_S_END;
	m_sem1.release(QUEUE_LENGTH);  // 释放所有槽位
	if(isRunning()){
		bool finished = wait(3000);
		if(!finished){
			terminate();
			wait();
		}
	}
	该函数段可以正确等待线程安全退出，没有程序崩溃错误
-> 线程申请和消费资源，当没有资源可以申请时，解码线程被阻塞，等待有资源时继续进行。多线程或者其他类似情况，生产消费模型很重要，如果不使用可能会出现 界面卡死、内存泄漏、播放速度失控、多线程崩溃等情况
-> 可以先把OnstartPlay 和 Ontimeout 两个槽函数的逻辑实现 ： 获取第一帧 和 持续获取

->在显示前需要测试转换是否成功，图片是否有变化，在OnstartPlay函数的AVData *tmp = m_pAVCtrl->GetData();
													m_pTimer->stop();
													if(tmp == NULL){
														return;
													}和Ontimeout同上后加：
	saveImageToFile(tmp->m_image);
	//保存图片到指定文件夹，查看转换是否成功
void Widget::saveImageToFile(QImage* image) {
    if (!image || image->isNull()) {
            qDebug() << "图片无效，跳过保存";
            return;
        }

        static int frameCount = 0;

        // 直接指定完整路径
        QString filePath = QString("D:/Qt/progect_video/demo_video/video_frames/frame_%1.png")
                            .arg(frameCount++, 4, 10, QChar('0'));

        // 确保目录存在
        QDir dir("D:/Qt/progect_video/demo_video/video_frames");
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        if (image->save(filePath)) {
            qDebug() << "✅ 保存成功:" << filePath;
        } else {
            qDebug() << "❌ 保存失败:" << filePath;
        }
}

-> 没问题就开始显示 -> 在 OnstartPlay 函数里 检查类型是否为视频类型，是的就进行显示
202604170254 完成视频流的初始化及视频播放
202604170257 使用栅格布局优化视频显示
202604170307 开始音频流的初始化及播放
202604171355 发现一个问题，点击播放按钮后，正常播放，可重复点击后，程序崩溃结束。解决方法：最简单的定义一个静态变量，记录是否在播放视频
if(isProcessing){
	qDebug() << "视频正在播放，请勿重复点击";
	return;
}
isProcessing = true; 加载打开文件前
202604171355 音频卡卡的
202604171645 音频卡卡的，不管了，完成了视频的播放。demo_devio 完成  魔改了，功能和例子一样了，就是音频卡、文件切换时出现程序崩溃，多次点打开文件的按钮也会


-> 解码线程 av_read_frame -> 流类型 ： 音频流 -> 音频解码 -> swr_convert 重采样 -> 写入音频设备 -> 音频播放

----------------------------------------------------------------------------------------------------------------------------------
202604171713 开始主要解决音频卡卡的问题 

202604180741 下个git用来备份文件，可以回退。在文件比较器里比较我写的和老师的例子，看看哪块逻辑不同。再改改



   			
						
						
						
						
						