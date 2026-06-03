#include <QDebug>
#include <QTime>
#include <QStyle>
#include "common.h"
#include "controlbar.h"
#include "ui_controlbar.h"
#include "av_control.h"


ControlBar::ControlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlBar)
{
    ui->setupUi(this);
    m_Total = 0;
    m_bPress = false;
    m_iCount = 0;
    m_iStart = 0;
    m_iEnd = 0;
    ui->label->setText("");
    ui->label_2->setText(SOFT_VERSION);
    ui->bt_play->setProperty("play", "start");
    ui->bt_volume->setProperty("setup", "normal");
    ui->bt_setup->hide();
    ui->play_process->installEventFilter(this);
}

ControlBar::~ControlBar()
{
    delete ui;
}

void ControlBar::SetPlayTotalTime(uint32_t total)
{
    FUNCTION_ENTER;
    m_Total = total;
    //进度条
    ui->play_process->setMaximum(m_Total);
    ui->play_process->setMinimum(0);
    ui->play_process->setPageStep(100);//步长0.1s，当用户按下键盘上的 PageUp / PageDown 键时，进度条会以这个幅度跳跃
    ui->play_process->setValue(0);//初始化0

    //声音条
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setPageStep(1);//当用户按下键盘上的 PageUp / PageDown 键时，进度条会以这个幅度跳跃
    ui->volumeSlider->setValue(50);

    ui->bt_play->setProperty("play","stop");
    ui->bt_play->style()->polish(ui->bt_play);//强制刷新
    FUNCTION_EXIT;
}

void ControlBar::SetPlayTime(uint32_t current)//随视频更新进度条
{
    FUNCTION_ENTER;
    if(m_bPress == false){//判断是否按压
        if(m_iCount > 0){
            m_iCount --;
            return;
        }
        qDebug() << "total " << m_Total << " cur " << current;
        if(m_iStart > 0 && m_iEnd > 0){//跳转后进度同步
            if(current > m_iEnd){
                ui->play_process->setValue(current);
                qDebug() << "start " << m_iStart << " end " << m_iEnd;
                m_iStart = 0;
                m_iEnd = 0;
            }
        }
        else{
            ui->play_process->setValue(current);
        }

        QString strTotal = QTime(0,0,0,0).addMSecs((int)m_Total).toString("HH:mm:ss");
        QString strCur = QTime(0,0,0,0).addMSecs((int)current).toString("HH:mm:ss");
        ui->label->setText(strCur + "/" + strTotal);

        if(strCur == strTotal){
            qDebug() << "play end";
            ui->bt_play->setProperty("play","start");
            ui->bt_play->style()->polish(ui->bt_play);

            QVariant ret;
            emit ControlBarSignal(CONTROL_BAR_PLAY_END,ret);
        }
    }

    FUNCTION_EXIT;
}

void ControlBar::ControlBarReset()
{
    FUNCTION_ENTER;
    m_Total = 0;
    m_bPress = false;
    m_iCount = 0;
    m_iStart = 0;
    m_iEnd = 0;
    ui->label->setText("");
    ui->bt_play->setProperty("play","statr");
    ui->bt_play->style()->polish(ui->bt_play);

    FUNCTION_EXIT;
}

void ControlBar::on_bt_play_clicked()
{
    FUNCTION_ENTER;
    QString str = ui->bt_play->property("play").toString();
    if(str == "start"){
        QVariant ret;
        emit ControlBarSignal(CONTROL_BAR_PLAY,ret);
    }
    else if(str == "stop"){
        ui->bt_play->setProperty("play","continue");
        ui->bt_play->style()->polish(ui->bt_play);
        STAVControl::GetInstance()->PausePlay();
    }
    else if(str == "continue"){
        ui->bt_play->setProperty("play","stop");
        ui->bt_play->style()->polish(ui->bt_play);
        STAVControl::GetInstance()->ContinuePlay();
    }
    FUNCTION_EXIT;
}


void ControlBar::on_bt_setup_clicked()
{
    FUNCTION_ENTER;
    QVariant ret;
    emit ControlBarSignal(CONTROL_BAR_SHOW_SETUP,ret);
    FUNCTION_EXIT;
}


void ControlBar::on_bt_next_clicked()
{
    FUNCTION_ENTER;
    QVariant ret;
    emit ControlBarSignal(CONTROL_BAR_NEXT_PLAY,ret);
    FUNCTION_EXIT;
}


void ControlBar::on_bt_previous_clicked()
{
    FUNCTION_ENTER;
    QVariant ret;
    emit ControlBarSignal(CONTROL_BAR_PRE_PLAY,ret);
    FUNCTION_EXIT;
}


void ControlBar::on_bt_volume_clicked()
{
    FUNCTION_ENTER;
    //loudspeaker mute
    QString str = ui->bt_volume->property("setup").toString();
    qDebug() << "bt_volume property " << str;
    if(str == "normal"){
        ui->bt_volume->setProperty("setup","mute");
        ui->bt_volume->style()->polish(ui->bt_volume);
        STAVControl::GetInstance()->SetVolume(0);
    }
    else{
        ui->bt_volume->setProperty("setup","normal");
        ui->bt_volume->style()->polish(ui->bt_volume);
        int val = ui->volumeSlider->value();
        STAVControl::GetInstance()->SetVolume(val);
    }

    FUNCTION_EXIT;
}


void ControlBar::on_volumeSlider_valueChanged(int value)
{
    FUNCTION_ENTER;
    STAVControl::GetInstance()->SetVolume(value);
    FUNCTION_EXIT;
}

void ControlBar::on_play_process_sliderPressed()//开始拖拽
{
    m_bPress = true;
    m_iCount = 0;
    m_iStart = ui->play_process->value();//记录拖拽前进度值
}

void ControlBar::on_play_process_sliderReleased()//结束拖拽
{
    FUNCTION_ENTER;
    if(!m_bPress){
        return;
    }
    m_bPress = false;

    qDebug() << ui->play_process->value();
    m_iEnd = ui->play_process->value();
    STAVControl::GetInstance()->SeekPlay(ui->play_process->value());

    m_iCount = 15;
    FUNCTION_EXIT;
}

bool ControlBar::eventFilter(QObject *obj, QEvent *event)
{
    if(obj != ui->play_process){
        return false;
    }
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent *ev = static_cast<QMouseEvent*>(event);
        if(ev->button() == Qt::LeftButton){
            int currentX = ev->pos().x();
            double per = currentX * 1.0 / ui->play_process->width();
            int value = per * (ui->play_process->maximum() - ui->play_process->minimum()) + ui->play_process->minimum();
            ui->play_process->setValue(value);
        }
    }
    return QWidget::eventFilter(obj, event);
}
