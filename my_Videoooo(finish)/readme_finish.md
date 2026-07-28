登录密码：Stkj180724     y20040308   现在的密码：w20060819
用户名：15291373954

https://github.com/deerlet354/My_code_pool


网络->UI->集成

QIODevice::WriteOnly | QIODevice::Truncate 表示打开文件用于写入，且从头开始写（覆盖原内容）。
| 在这里是“标志组合器”，将多个独立的选项合并成一个整数传递给函数。

202605272230 登录完成
202605282248 获取播放列表完成
202605291515 用户数据加密、自动登录、下载文件、下载进度完成
202605291844 录屏检测、用户水印完成
202606011424 播放视频完成			
202606011741 进度条随视频刷新、视频列表的隐藏显示和刷新、控制按钮的开始暂停、拖拽进度条跳转 完成
202606031654 密码重置、退出登录、下一个视频上一个视频切换、声音条、点击进度条跳转 完成
202606042210 音视频同步完成
	
音视频同步方法：外部时间基准，通过时间偏移量计算下一帧时间误差，每次加上误差，每次校准下一帧播放超时

音频卡的原因是帧间隔时间用计算的len的话是卡的，用固定时间（见代码）是正常的

核心逻辑
		playtime = m_playtimeList.front();//下一帧播放时间
        int elapsed = m_timer.elapsed();//距上一次开始计时，程序运行到这一步过去的时间
        qDebug() << "base tmie" << m_timeval << " offset " << elapsed;
        int offset = 0;

        m_timeval += elapsed; //时间基准，一直累加
        m_timer.restart();
        offset = tmp->m_iPlayTime - m_timeval;//上一帧播放时间 - 时间基准
                                              //比如上帧时间50ms，程序运行到m_timeval += elapsed 是 53ms
                                              //则 时间误差是50 - 53 = -3

        len = playtime - tmp->m_iPlayTime + offset;   //那么超时 100 - 50 -3 = 47ms播放下一帧，后面同样逻辑
        if(qAbs(len) > 500){
            m_timeval = playtime;
            len = 0;
        }
        qDebug() << "timeout nextplaytime " << playtime << " " << m_timeval
                 << " cur_playtime " << tmp->m_iPlayTime << " len " << len;
        m_playtimeList.pop_front();

					
202606042223  项目结束！



