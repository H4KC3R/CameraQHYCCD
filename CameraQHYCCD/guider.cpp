#include <iostream>
#include "guider.h"
#include "ui_guider.h"
#include "qhyccd.h"
//#include "setparamdialog.h"
//#include "ui_setparamdialog.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <QDateTime>
#include <QDebug>
#include <QWheelEvent>
#include <QFile>
#include "mycombobox.h"
#include "math.h"

using namespace std;

bool flag_live_quit = false;
bool flag_lived     = false;
bool flag_getlive   = false;

int fps = 0,avr_fps = 0,t_num = 0;
int t_start,t_end;
int length;

unsigned char *ImgData;

uint32_t imagew,imageh;
uint32_t bpp;
uint32_t channels;

char type[30];

pthread_t tid_live;
qhyccd_handle *camhandle;

//void Guider::wheelEvent(QWheelEvent *event)
//{
//    printf("*****\n");
//}

Guider::Guider(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Guider)
{
    ui->setupUi(this);

    canGetTemp = true;

//    QDateTime current_date_time =QDateTime::currentDateTime();
//    QString current_date =current_date_time.toString("yyyyMMddhhmmss");
//    qDebug() << current_date;

//    QString lutPath = QCoreApplication::applicationDirPath() + "/LUT.png";
//    lutImgSrc = cvLoadImage(lutPath.toLatin1().data());
////    cvNamedWindow("LUT",0);
////    cvShowImage("LUT",lutImgSrc);
//    printf("width = %d height = %d depth = %d widthStep = %d nChannels = %d\n",lutImgSrc->width,lutImgSrc->height,lutImgSrc->depth,lutImgSrc->widthStep,lutImgSrc->nChannels);
//    for(int i = 0; i < lutImgSrc->width; i ++){
//        if((i+1) % 5 == 0)
//            printf("%4d, %4d, %4d\n",lutImgSrc->imageData[3*i],lutImgSrc->imageData[3*i+1],lutImgSrc->imageData[3*i+2]);
//        else
//            printf("%4d, %4d, %4d, ",lutImgSrc->imageData[3*i],lutImgSrc->imageData[3*i+1],lutImgSrc->imageData[3*i+2]);
//    }
//    printf("\n");

//    for(int i = -89; i < 90; i ++){
//        if(i % 10 == 0)
//            printf("%10f, \n",tan((double)i / 180 * M_PI));
//        else
//            printf("%10f, ",tan((double)i / 180 * M_PI));
//    }

    QString logoPath = QCoreApplication::applicationDirPath() + "/logo/logo.png";
    if(QFile::exists(logoPath)){
        IplImage *iconImgSrc = cvLoadImage(logoPath.toLatin1().data());
        int w = ui->icn_lbl->width();
        int h = ui->icn_lbl->height();
        int channels = iconImgSrc->nChannels;
        IplImage *iconImgDst = cvCreateImage(cvSize(w,h),iconImgSrc->depth,channels);
        cvResize(iconImgSrc,iconImgDst,CV_INTER_AREA);

        QImage *iconImg = new QImage(w,h, QImage::Format_RGB888);
        char *data = iconImgDst->imageData;
        for (int y = 0; y < h; y++, data += iconImgDst->widthStep)
        {
            for (int x = 0; x < w; x++)
            {
                if(channels == 3)
                {
                    char r, g, b;//, a = 0;
                    r = data[x * channels + 2];
                    g = data[x * channels + 1];
                    b = data[x * channels];
                    iconImg->setPixel(x, y, qRgb(r, g, b));
                }
                else
                {
                    char r, g, b;//, a = 0;
                    r = data[x * channels];
                    g = data[x * channels];
                    b = data[x * channels];
                    iconImg->setPixel(x, y, qRgb(r, g, b));
                }
            }
        }
        ui->icn_lbl->setPixmap(QPixmap::fromImage(*iconImg));
        ui->icn_lbl->show();

        cvReleaseImage(&iconImgSrc);
        cvReleaseImage(&iconImgDst);
    }

    //printf();
//    comboBox = new MyComboBox(ui->groupBox_16);
//    comboBox->addItem("1");
//    comboBox->addItem("2");
//    comboBox->addItem("3");
//    comboBox->addItem("4");
//    QFont f2;
//    f2.setBold(false);
//    f2.setFamily("SimSun");
//    comboBox->setFont(f2);
//    comboBox->setGeometry(90,50,131,22);

    QFont f;
    f.setBold(true);
    f.setPixelSize(15);
    f.setFamily("Arial");
    ui->pushButton_2->setFont(f);
    ui->pushButton_3->setFont(f);
    ui->pushButton_4->setFont(f);
    ui->pushButton_5->setFont(f);
    ui->pushButton_6->setFont(f);
    ui->pushButton_7->setFont(f);
    ui->pushButton_8->setFont(f);
    ui->pushButton_9->setFont(f);
    ui->pushButton_10->setFont(f);

    b1=b2=b3=b4=b5=b6=b7=b8=-1;
    ui->pushButton_2->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_3->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_4->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_5->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_6->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_7->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_8->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->pushButton_9->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
    ui->groupBox->setVisible(false);
    ui->groupBox_2->setVisible(false);
    ui->groupBox_3->setVisible(false);
    ui->groupBox_4->setVisible(false);
    ui->groupBox_5->setVisible(false);
    ui->groupBox_6->setVisible(false);
    ui->groupBox_7->setVisible(false);
    ui->groupBox_8->setVisible(false);

//    ui->groupBox_12->set>setTitle("<html><head/><body><p><span style='font:bold;'>InitQHYCCDResource()</span></p></body></html>");

    QFont f1;
    f1.setPixelSize(17);
    f1.setFamily("Arial");
    ui->label_2->setFont(f1);
    ui->label_3->setFont(f1);

    ui->verticalLayout->addStretch();

    /*设置鼠标悬浮时的显示文本，可以将按键对应的功能函数显示出来*/
    ui->intr_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>InitQHYCCDResource():</span>"
                                 "This function can initialize SDK resource,please call it in the front of program.</p></body></html>"));//"InitQHYCCDResource");
    ui->reles_btn->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>ReleaseQHYCCDResource():</span>"
                                 "This function can release SDK resource,please call it in the end of program.</p></body></html>"));//"ReleaseQHYCCDResource");
//    ui->osxf_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>OSXInitQHYCCDFirmware():</span>"
//                                 "This function can download firmware to camera,just need use it on Mac.</p></body></html>"));//"OSXInitQHYCCDFirmware");
    ui->scan_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>ScanQHYCCD():</span>"
                                 "This function can scan QHYCCD camera and return device number.</p></body></html>"));//"ScanQHYCCD");
    ui->open_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>OpenQHYCCD():</span>"
                                 "This function can open QHYCCD camera and return a device handle.</p></body></html>"));//"OpenQHYCCD");
    ui->intq_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>InitQHYCCD():</span>"
                                 "This function can initialize camera resource and set initial value to camera's parameters.</p></body></html>"));//"InitQHYCCD");
    ui->clos_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>CloseQHYCCD():</span>"
                                 "This function can close QHYCCD camera,when you finish control camera you need close camera with this function.</p></body></html>"));//"CloseQHYCCD");
    ui->gtid_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDId():</span>"
                                 "This function can get camera's ID,the ID contain camera model and serval number.</p></body></html>"));//"GetQHYCCDId");
    ui->fwvs_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDFWVersion():</span>"
                                 "This function can get camera's firmware(driver) version.</p></body></html>"));//"GetQHYCCDFWVersion");
    ui->sdkv_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDSDKVersion():</span>"
                                 "This function can get SDK version.</p></body></html>"));//"GetQHYCCDSDKVersion");
    ui->mmsp_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDParamMinMaxStep():</span>"
                                 "This function can get parameters minimum,maximum and step,you can get parameters setting range and step.</p></body></html>"));//"GetQHYCCDParamMinMaxStep");
    ui->gtcn_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDParam(qhyccd_handle *handle,CONTROL_CFWSLOTSNUM):</span>"
                                 "This function can get color filter wheel slots number.</p></body></html>"));//"GetQHYCCDParam");
    ui->gtci_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDChipInfo():</span>"
                                 "This function can get camera chip information,this information is about camera hardware.</p></body></html>"));//"GetQHYCCDChipInfo");
    ui->eftv_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDEffectiveArea():</span>"
                                 "This function can get effective area range in image.</p></body></html>"));//"GetQHYCCDEffectiveArea");
    ui->gtos_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDOverScanArea():</span>"
                                 "This function can get overscan area range in image.</p></body></html>"));//"GetQHYCCDOverScanArea");
    ui->isal_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>IsQHYCCDControlAvailable():</span>"
                                 "This function can check if a function is supported by camera.</p></body></html>"));//"IsQHYCCDControlAvailable");
    ui->gtpm_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDParam():</span>"
                                 "This function can get parameter value,for example, gain, offset...</p></body></html>"));//"GetQHYCCDParam");
    ui->mlth_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDMemLength():</span>"
                                 "This function can get memory length of one frame.</p></body></html>"));//"GetQHYCCDMemLength");
//    ui->dboo_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDDebayerOnOff():</span>"
//                                 "This function can set camera work on color mode or mono mode.</p></body></html>"));//"SetQHYCCDDebayerOnOff");
//    ui->stpm_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDParam():</span>"
//                                 "This function can set parameter to camera,for example, gain, offset...</p></body></html>"));//"SetQHYCCDParam");
//    ui->stfm_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDFrameMode():</span>"
//                                 "This function can set camera work on single mode or live mode.</p></body></html>"));//"SetQHYCCDFrameMode");
//    ui->stbm_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDBinMode():</span>"
//                                 "This function can set camera bin mode,for example, 1X1, 2X2...</p></body></html>"));//"SetQHYCCDBinMode");
//    ui->strl_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDResolution():</span>"
//                                 "This function can set image resolution or ROI.</p></body></html>"));//"SetQHYCCDResolution");
    ui->epsg_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>ExpQHYCCDSingleFrame():</span>"
                                 "This function can begin expose one frame in single mode.</p></body></html>"));//"ExpQHYCCDSingleFrame");
    ui->gtsg_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDSingleFrame():</span>"
                                 "This function can get one frame data in single mode.</p></body></html>"));//"GetQHYCCDSingleFrame");
    ui->cerd_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>CancelQHYCCDExposingAndReadout():</span>"
                                 "This function can cancel expose in single mode.</p></body></html>"));//"CancelQHYCCDExposingAndReadout");
//    ui->cexp_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>CancelQHYCCDExposing():</span>"
//                                 "This function can cancel expose in single mode.</p></body></html>"));//"CancelQHYCCDExposing");
    ui->bglv_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>BeginQHYCCDLive():</span>"
                                 "This function can begin expose in live mode.</p></body></html>"));//"BeginQHYCCDLive");
    ui->gtlv_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDLiveFrame():</span>"
                                 "This function can get image data in live mode.</p></body></html>"));//"GetQHYCCDLiveFrame");
    ui->stpl_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>StopQHYCCDLive():</span>"
                                 "This function can stop expose in live mode.</p></body></html>"));//"StopQHYCCDLive");
    ui->cfwp_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>IsQHYCCDCFWPlugged():</span>"
                                 "This function can check if color filter wheel has been plugged.</p></body></html>"));//"IsQHYCCDCFWPlugged");
    ui->cfws_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDCFWStatus():</span>"
                                 "This function can get current color filter wheel status.</p></body></html>"));//"GetQHYCCDCFWStatus");
    ui->odtc_cbbx->setToolTip(tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SendOrder2QHYCCDCFW():</span>"
                                 "This function can send control order to color filter wheel.</p></body></html>"));//"SendOrder2QHYCCDCFW");
    ui->sttp_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>ControlQHYCCDTemp():</span>"
                                 "This function can set a target temperature to camera.</p></body></html>"));
    ui->gttp_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDParam(qhyccd_handle *handle,CONTROL_CURTEMP):</span>"
                                 "This function can get current temperature from camera.</p></body></html>"));
    ui->spwm_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>SetQHYCCDParam(qhyccd_handle *handle,CONTROL_MANULPWM,pwm):</span>"
                                 "This function can set a target pwm to camera.</p></body></html>"));
    ui->gpwm_btn->setToolTip( tr("<html><head/><body><p><span style='color:#000000; font:bold;'>GetQHYCCDParam(qhyccd_handle *handle,CONTROL_CURPWM):</span>"
                                 "This function can get current pwm from camera.</p></body></html>"));

//    connect(ui->ost_cbx,&QComboBox::activated,this,&Guider::wheelEvent);
    /*Qt的语法，将按键事件与定义好的函数联系起来*/
    //QPushButton是按键的类型，clicked是按键的点击事件
    connect(ui->intr_btn,&QPushButton::clicked,this,&Guider::InitResource);//初始化
    //connect(ui->osxf_btn,&QPushButton::clicked,this,&Guider::OSXInitFirmware);//Mac下加载固件
    connect(ui->scan_btn,&QPushButton::clicked,this,&Guider::ScanCamera);//扫描相机
    connect(ui->gtid_btn,&QPushButton::clicked,this,&Guider::GetId);//获取相机ID
    connect(ui->open_btn,&QPushButton::clicked,this,&Guider::Open);//打开相机
    connect(ui->intq_btn,&QPushButton::clicked,this,&Guider::Init);//初始化相机
    connect(ui->clos_btn,&QPushButton::clicked,this,&Guider::Close);//关闭相机
    connect(ui->reles_btn,&QPushButton::clicked,this,&Guider::ReleaseResource);//释放资源
    connect(ui->fwvs_btn,&QPushButton::clicked,this,&Guider::FirmWareVersion);//获取固件版本
    connect(ui->sdkv_btn,&QPushButton::clicked,this,&Guider::SDKVersion);//获取SDK版本
    connect(ui->mmsp_cbbx,SIGNAL(activated(int)),this,SLOT(MinMaxStep()));//获取可设置参数的最大最小值和设置步长
    connect(ui->gtci_btn,&QPushButton::clicked,this,&Guider::GetChipInfo);//获取相机片上信息
    connect(ui->mlth_btn,&QPushButton::clicked,this,&Guider::MemLength);//获取相机图像数据的长度
    /*activated是ComboBox的单击事件，即使下标不发生改变也会执行相应操作，有int和string两种类型，使用时需要指明类型，否则会报错*/
    connect(ui->gtpm_cbbx,SIGNAL(activated(int)),this,SLOT(GetParam()));//获取相机参数设置值
    connect(ui->isal_cbbx,SIGNAL(activated(int)),this,SLOT(ControlAvailable()));
    connect(ui->eftv_btn,&QPushButton::clicked,this,&Guider::GetEffectiveArea);//获取有效区域
    connect(ui->gtos_btn,&QPushButton::clicked,this,&Guider::GetOverscanArea);//获取过扫区
    connect(ui->clr_cbx,SIGNAL(activated(int)),this,SLOT(SetDebayerOnOff()));//设置彩色模式开关
//    connect(tdl->ui->dok_btn,&QPushButton::clicked,this,&Guider::on_dok_btn_clicked);//设置温控对话框OK按键点击时的操作
    connect(ui->stfm_cbbx,SIGNAL(activated(int)),this,SLOT(SetStreamMode()));//设置单帧连续模式
//    connect(ui->stpm_cbbx,SIGNAL(activated(int)),this,SLOT(SetParam()));//设置相机参数
//    connect(sdlg->ui->sp_valu_sld,SIGNAL(valueChanged(int)),this,SLOT(SpinBoxShowNum()));//设置滚动条与SpinBox数值保持同步
//    connect(sdlg->ui->sp_valu_spb,SIGNAL(valueChanged(double)),this,SLOT(SliderShowNum()));//设置滚动条与SpinBox数值保持同步
//    connect(sdlg->ui->sp_unit_btn,&QPushButton::clicked,this,&Guider::SPUnitBtnClicked);
//    connect(sdlg->ui->sp_ok_btn,&QPushButton::clicked,this,&Guider::SPOKBtnClicked);

//    connect(ui->stbm_cbbx,SIGNAL(activated(int)),this,SLOT(SetBinMode()));
//    connect(ui->strl_btn,&QPushButton::clicked,this,&Guider::SetResolutin);
    connect(ui->epsg_btn,&QPushButton::clicked,this,&Guider::ExpSingleFrame);
    connect(ui->gtsg_btn,&QPushButton::clicked,this,&Guider::GetSingleFrame);

    connect(ui->cerd_btn,&QPushButton::clicked,this,&Guider::CancelExposingAndReadout);
//    connect(ui->cexp_btn,&QPushButton::clicked,this,&Guider::CancelExposing);

    connect(ui->bglv_btn,&QPushButton::clicked,this,&Guider::BeginLive);
    connect(ui->gtlv_btn,&QPushButton::clicked,this,&Guider::GetLiveFrame);
    connect(ui->stpl_btn,&QPushButton::clicked,this,&Guider::StopLiveFrame);

    connect(ui->cfwp_btn,&QPushButton::clicked,this,&Guider::IsCFWPlugged);
    connect(ui->cfws_btn,&QPushButton::clicked,this,&Guider::GetCFWStatus);
    connect(ui->gtcn_btn,&QPushButton::clicked,this,&Guider::GetCFWSlotsNum);
    connect(ui->odtc_cbbx,SIGNAL(activated(int)),this,SLOT(SendOrder2CFW()));

//    connect(update_timer,SIGNAL(timeout()),this,SLOT(update()));
}

Guider::~Guider()
{
    delete ui;
}

//void Guider::update()
//{
//    double temp = GetQHYCCDParam(camhandle,CONTROL_CURTEMP);
//    ui->opt_tbs->clear();
//    ui->opt_tbs->setText("Introduction:\n"
//                         "    Current temperature is " + QString::number(temp,16,1) + "°C.You can use 'temp = GetQHYCCDParam("
//                         "camhandle,CONTROL_CURTEMP);' get it");
//}

//void Guider::timerEvent(QTimerEvent *event)
//{
//    if()
//}

/*定时器函数，用来计算连续模式下的帧率和持续时间*/
void Guider::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == timer && canGetTemp == true){
        double value = GetQHYCCDParam(camhandle,CONTROL_CURTEMP);
        ui->opt_tbs->setText(QString::number(value,16,1) + " °C");
    }
}

/*获取SDK版本，因为这个函数只是取出SDK中记录的版本日期，所以与其他按键没有互动*/
void Guider::SDKVersion()
{
    ret = GetQHYCCDSDKVersion(&year,&month,&day,&subday);
    if(ret == QHYCCD_SUCCESS)
    {
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret;\n"
                             "uint32_t year,month,day,subday;\n"
                             "ret = GetQHYCCDSDKVersion(&year,&month,&day,&subday);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get SDK's version,if function run successfull,it will return 'QHYCCD_SUCCESS'.You can check SDK if is newest "
                             "by its version.And this function is independent,you don't need run any other function before you run this function.\n"
                             "\n"
                             "Function run result:\n"
                             "SDK version:"
                             + QString::number(year,16,0) + "-"
                             + QString::number(month,16,0) + "-"
                             + QString::number(day,16,0) + ","
                             + QString::number(subday,16,0));
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret;\n"
                             "uint32_t year,month,day,subday;\n"
                             "ret = GetQHYCCDSDKVersion(&year,&month,&day,&subday);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get SDK's version,if function run successfull,it will return 'QHYCCD_SUCCESS'.You can check SDK if is newest "
                             "by its version.And this function is independent,you don't need run any other function before you run this function.\n"
                             "\n"
                             "Function run result:\n"
                             "Get SDK version failed.");
    }
}

/*初始化资源，相机一系列操作中最开始的操作*/
void Guider::InitResource()
{
    ret = InitQHYCCDResource();
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret;\n"
                             "ret = InitQHYCCDResource();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can initialize SDK resource,you need run this function at the program start,and only need run this function one time,"
                             "otherwise will make program broken.If function run successfully,its return value is 'QHYCCD_SUCCESS',if error,its return value is "
                             "'QHYCCD_ERROR'.\n"
                             "\n"
                             "Function run result:\n"
                             "Initializing QHYCCD SDK resource successfully.\n"
                             "Please click 'ScanQHYCCD' button to do next operation.");
#ifdef Q_OS_MAC
        //Mac平台下，初始化完成后需要为相机加载固件，所以使能加载固件按键
        ui->osxf_btn->setEnabled(true);
        ui->intr_btn->setEnabled(false);
#else
        //Windows平台下，初试完后不需要加载固件，因为有驱动，可以直接开始扫描相机，不过有些情况下未加载固件可以使用函数加载固件
        ui->scan_btn->setEnabled(true);
        ui->intr_btn->setEnabled(false);
#endif
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret;\n"
                             "ret = InitQHYCCDResource();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can initialize SDK resource,you need run this function at the program start,and only need run this function one time,"
                             "otherwise will make program broken.If function run successfully,its return value is 'QHYCCD_SUCCESS',if error,its return value is "
                             "'QHYCCD_ERROR'.\n"
                             "\n"
                             "Function run result:\n"
                             "Initializing QHYCCD SDK resource fail.");
    }
}

/*Mac平台下加载固件的函数，是Mac下必不可少的一步*/
void Guider::OSXInitFirmware()
{
#ifdef Q_OS_MAC
    char path[] = "/usr/local";//固件的存放路径，写到firmware上一级目录就可以，固件放在firmware目录下
    ret = OSXInitQHYCCDFirmware(path);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("OSX init firmware successfully.");
        ui->osxf_btn->setEnabled(false);
        ui->scan_btn->setEnabled(true);//加载完固件后可以执行扫描相机操作
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("OSX init firmware failed.");
    }
#endif
}

/*扫描相机*/
void Guider::ScanCamera()
{
    num = ScanQHYCCD();
    if(num >= 1){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int num = 0;\n"
                             "num = ScanQHYCCD();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get the number of the camera that you connect to computer,and return num to you.Please just connect 1 camera "
                             "when you use this software,because I didn't add judgement for multiple cameras.\n"
                             "\n"
                             "Function run result:\n"
                             "Finded " + QString::number(num,16,0) + " camera\n"
                             "Please click 'GetId' button to do next operation.");
        ui->scan_btn->setEnabled(false);
        ui->gtid_btn->setEnabled(true);//扫描到相机之后可以获取相机ID
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int num = 0;\n"
                             "num = ScanQHYCCD();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get the number of the camera that you connect to computer,and return num to you.Please just connect 1 camera "
                             "when you use this software,because I didn't add judgement for multiple cameras.\n"
                             "\n"
                             "Function run result:\n"
                             "Can't find QHYCCD camera,please check USB link.");
    }
}

/*获取扫描到的相机的ID*/
void Guider::GetId()
{
    for(int i = 0;i < num;i ++)
    {
        ret = GetQHYCCDId(i,id);
        if(ret == QHYCCD_SUCCESS){
            ret = GetQHYCCDModel(id,cam.model);
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret;\n"
                                 "char id[32];\n"
                                 "ret = GetQHYCCDId(i,id);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can get camera's ID,this ID can mark camera,you need use this ID open camera later.'num' is the camera number "
                                 "that you get by 'ScanQHYCCD()''.If function run successfully,its return value is 'QHYCCD_SUCCESS',if error,its return value is "
                                 "'QHYCCD_ERROR'.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Get camera ID successfully.Its ID is\n" + QString(QLatin1String(id)) + "\n"
                                 "Please click 'OpenQHYCCD' button to do next operation.");//char*转换成QString
            ui->gtid_btn->setEnabled(false);
            ui->open_btn->setEnabled(true);//获取ID后可以打开相机
            break;//由于暂未实现多相机功能，所以在获取第一个QHYCCD设备ID之后就会停止
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret;\n"
                                 "char id[32];\n"
                                 "ret = GetQHYCCDId(i,id);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can get camera's ID,this ID can mark camera,you need use this ID open camera later.'num' is the camera number "
                                 "that you get by 'ScanQHYCCD()''.If function run successfully,its return value is 'QHYCCD_SUCCESS',if error,its return value is "
                                 "'QHYCCD_ERROR'.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Get camera ID fail.");
        }
    }
}

/*打开相机，需要用到之前获取的相机ID*/
void Guider::Open()
{
    camhandle = OpenQHYCCD(id);
    if(camhandle != NULL)
    {
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "qhyccd_handle *camhandle = NULL;\n"
                             "camhandle = OpenQHYCCD(id);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can use ID open camera,and return camera's handle,the ID can get by 'GetQHYCCDId()'.If successfully,'camhandle'' is not NULL,"
                             "if error,'camhandle' is NULL.\n"
                             "\n"
                             "Function run result:\n"
                             "Open camera successfully.\n"
                             "Please click 'FrameMode' button to do next operation.");

        ui->open_btn->setEnabled(false);
        ui->stfm_cbbx->setEnabled(true);//使能选择相机工作模式的按键
        ui->fwvs_btn->setEnabled(true);//使能获取固件版本的按键
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "qhyccd_handle *camhandle = NULL;\n"
                             "camhandle = OpenQHYCCD(id);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can use ID open camera,and return camera's handle,the ID can get by 'GetQHYCCDId()'.If successfully,'camhandle'' is not NULL,"
                             "if error,'camhandle' is NULL.\n"
                             "\n"
                             "Function run result:\n"
                             "Open camera fail.");
    }
}

/*初始化相机资源，初始化后直接判断相机具有的功能*/
void Guider::Init()
{
    ret = InitQHYCCD(camhandle);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = InitQHYCCD(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can new a class for camera,this class include parameter and other resource,and you "
                             "only need use this function one time,if you use this function more time,the function will create "
                             "new classes,these classes will make program confuse which class is it need.'camhandle' is getted by "
                             "'OpenQHYCCD()'.\n"
                             "\n"
                             "Function run result:\n"
                             "Initializing successfully.\n"
                             "Now camera has been connected,you can control it.");

        /*初始化相机资源后大部分的功能按键都可以使用了*/
        ui->intq_btn->setEnabled(false);
        ui->clos_btn->setEnabled(true);//使能关闭按键
        ui->mmsp_cbbx->setEnabled(true);//使能最大最小值及步长获取下拉列表
        ui->gtci_btn->setEnabled(true);//使能片上信息获取按键
        ui->isal_cbbx->setEnabled(true);//使能功能检测按键
        ui->gtpm_cbbx->setEnabled(true);//使能参数获取按键
        ui->eftv_btn->setEnabled(true);//使能有效区域获取按键
        ui->gtos_btn->setEnabled(true);//使能过扫区获取按键
        ui->mlth_btn->setEnabled(true);//使能图像数据长度获取按键
        ui->sttp_btn->setEnabled(true);
        ui->gttp_btn->setEnabled(true);
        ui->spwm_btn->setEnabled(true);
        ui->gpwm_btn->setEnabled(true);
        ui->bit_cbx->setEnabled(true);
        ui->bin_cbx->setEnabled(true);
        ui->stx_pbx->setEnabled(true);
        ui->sty_pbx->setEnabled(true);
        ui->szx_pbx->setEnabled(true);
        ui->szy_pbx->setEnabled(true);
        ui->roi_btn->setEnabled(true);
        ui->ost_cbx->setEnabled(true);

        ret = IsQHYCCDControlAvailable(camhandle,CAM_COLOR);//判断是否是彩色相机
        if(ret == BAYER_GB || ret == BAYER_GR || ret == BAYER_BG ||ret == BAYER_RG){
            ui->clr_cbx->setEnabled(true);//使能彩色模式开关按键
            isCvtColor = true;
            bayermatrix = ret;
            ret = SetQHYCCDDebayerOnOff(camhandle,false);
        }else{
            ui->clr_cbx->setEnabled(false);
        }

        ret = IsQHYCCDControlAvailable(camhandle,CONTROL_DDR);
        if(ret == QHYCCD_SUCCESS)
            ui->ddr_cbx->setEnabled(true);
        else
            ui->ddr_cbx->setEnabled(false);

        ui->exp_pbx->setMinimum(1);
        ui->exp_pbx->setMaximum(10000);
        ui->exp_pbx->setSingleStep(1);
        ui->exp_sld->setMinimum(1);
        ui->exp_sld->setMaximum(10000);
        ui->exp_sld->setPageStep(1);
        ui->exp_pbx->setEnabled(true);
        ui->exp_sld->setEnabled(true);
        ui->unt_btn->setEnabled(true);

        QSlider *sliderList[10]   = {
                                     ui->gan_sld,
                                     ui->oft_sld,
                                     ui->spd_sld,
                                     ui->tfc_sld,
                                     ui->wbb_sld,
                                     ui->wbg_sld,
                                     ui->wbr_sld,
                                     ui->brt_sld,
                                     ui->ctt_sld,
                                     ui->gma_sld};
        QDoubleSpinBox *spinboxList[10] = {
                                     ui->gan_pbx,
                                     ui->oft_pbx,
                                     ui->spd_pbx,
                                     ui->tfc_pbx,
                                     ui->wbb_pbx,
                                     ui->wbg_pbx,
                                     ui->wbr_pbx,
                                     ui->brt_pbx,
                                     ui->ctt_pbx,
                                     ui->gma_pbx};
        int idList[10] = {6,7,9,12,2,4,3,0,1,5};

        int i;
        double min,max,step;
        for(i = 0;i < 10;i ++){
            ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)idList[i]);
            if(ret == QHYCCD_SUCCESS){
                spinboxList[i]->setEnabled(true);
                sliderList[i]->setEnabled(true);

                ret = GetQHYCCDParamMinMaxStep(camhandle,(CONTROL_ID)idList[i],&min,&max,&step);
                if(ret == QHYCCD_SUCCESS){
                    spinboxList[i]->setMinimum(min);
                    spinboxList[i]->setMaximum(max);
                    spinboxList[i]->setSingleStep(step);


                    if(idList[i] == 0 || idList[i] == 1 || idList[i] == 5)
                        sliderList[i]->setMinimum(min * 10);
                    else
                        sliderList[i]->setMinimum(min);
                    if(idList[i] == 0 || idList[i] == 1 || idList[i] == 5)
                        sliderList[i]->setMaximum(max * 10);
                    else
                        sliderList[i]->setMaximum(max);
                    if(idList[i] == 0 || idList[i] == 1 || idList[i] == 5)
                        sliderList[i]->setPageStep(step * 10);
                    else
                        sliderList[i]->setPageStep(step);
                }
            }else{
                spinboxList[i]->setEnabled(false);
                sliderList[i]->setEnabled(false);
            }
        }

        //printf("%d\n",ui->bit_cbx->count());
        for(i = 0;i < ui->bit_cbx->count(); i ++){
            ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)(34 + i));
            if(ret == QHYCCD_SUCCESS){
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->bit_cbx->model()->index(i,1);//设置功能检测下拉选项使能
                ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
            }else{
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->bit_cbx->model()->index(i,0);//设置功能检测下拉选项不使能
                ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
            }
        }

        //printf("%d\n",ui->bin_cbx->count());
        for(i = 0;i < ui->bin_cbx->count(); i ++){
            ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)(21 + i));
            if(ret == QHYCCD_SUCCESS){
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->bin_cbx->model()->index(i,1);//设置功能检测下拉选项使能
                ui->bin_cbx->model()->setData(index,v,Qt::UserRole - 1);
            }else{
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->bin_cbx->model()->index(i,0);//设置功能检测下拉选项不使能
                ui->bin_cbx->model()->setData(index,v,Qt::UserRole - 1);
            }
        }

//        int idx;
        int idArray[11] = {13,36,37,38,39,40,41,42,43,49,50};
        for(i = 0;i < 11;i ++){
            ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)idArray[i]);
            if(ret == QHYCCD_SUCCESS){
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->ost_cbx->model()->index(i + 1,1);//设置功能检测下拉选项使能
                ui->ost_cbx->model()->setData(index,v,Qt::UserRole - 1);
            }else{
                QModelIndex index;//定义下拉列表选项的下标
                QVariant v(0);
                index = ui->ost_cbx->model()->index(i + 1,0);//设置功能检测下拉选项不使能
                ui->ost_cbx->model()->setData(index,v,Qt::UserRole - 1);
            }
        }

        ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);
        printf("%d %d\n",imagew,imageh);
        if(ret == QHYCCD_SUCCESS){
            cam.chipw = chipw;
            cam.chiph = chiph;
            cam.pixelw = pixelw;
            cam.pixelh = pixelh;
            cam.maximgw = imagew;
            cam.maximgh = imageh;

            ui->stx_pbx->setMinimum(0);
            ui->stx_pbx->setMaximum(imagew);

            ui->szx_pbx->setMinimum(0);
            ui->szx_pbx->setMaximum(imagew);

            ui->sty_pbx->setMinimum(0);
            ui->sty_pbx->setMaximum(imageh);

            ui->szy_pbx->setMinimum(0);
            ui->szy_pbx->setMaximum(imageh);

            ret = SetQHYCCDBinMode(camhandle,1,1);
            ret = SetQHYCCDResolution(camhandle,0,0,imagew/1,imageh/1);
        }

        ret = SetQHYCCDParam(camhandle,CONTROL_TRANSFERBIT,8);


//        QComboBox list[]
//        QModelIndex index;//定义下拉列表选项的下标
//        QVariant v(0);
//        index = ui->isal_cbbx->model()->index(1,1);//设置功能检测下拉选项使能
//        ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
//        index = ui->isal_cbbx->model()->index(1,0);//设置功能检测下拉选项不使能
//        ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
//        ui->stpm_cbbx->setEnabled(true);//使能参数设置按键
//        ui->stbm_cbbx->setEnabled(true);//使能BIN模式设置按键

        if(flag_single == true && flag_live == false){
            ui->tfc_pbx->setEnabled(false);
            ui->tfc_sld->setEnabled(false);
            ui->wbb_pbx->setEnabled(false);
            ui->wbb_sld->setEnabled(false);
            ui->wbg_pbx->setEnabled(false);
            ui->wbg_sld->setEnabled(false);
            ui->wbr_pbx->setEnabled(false);
            ui->wbr_sld->setEnabled(false);
            ui->brt_pbx->setEnabled(false);
            ui->brt_sld->setEnabled(false);
            ui->ctt_pbx->setEnabled(false);
            ui->ctt_sld->setEnabled(false);
            ui->gma_pbx->setEnabled(false);
            ui->gma_sld->setEnabled(false);

            ui->epsg_btn->setEnabled(true);
            ui->bglv_btn->setEnabled(false);

            ret = SetQHYCCDParam(camhandle,CONTROL_TRANSFERBIT,16);
            ui->bit_cbx->setCurrentIndex(1);

            ret = SetQHYCCDParam(camhandle,CONTROL_WBB,50);
            ui->wbb_pbx->setValue(50);
            ret = SetQHYCCDParam(camhandle,CONTROL_WBG,50);
            ui->wbg_pbx->setValue(50);
            ret = SetQHYCCDParam(camhandle,CONTROL_WBR,50);
            ui->wbr_pbx->setValue(50);

            ret = SetQHYCCDParam(camhandle,CONTROL_BRIGHTNESS,0);
            ui->brt_pbx->setValue(0);
            ret = SetQHYCCDParam(camhandle,CONTROL_CONTRAST,0);
            ui->ctt_pbx->setValue(0);
            ret = SetQHYCCDParam(camhandle,CONTROL_GAMMA,1.0);
            ui->gma_pbx->setValue(1.0);

            ret = SetQHYCCDDebayerOnOff(camhandle,false);
            ui->clr_cbx->setCurrentText("OFF");
        }else if(flag_single == false && flag_live == true){
            ui->tfc_pbx->setEnabled(true);
            ui->tfc_sld->setEnabled(true);
            ui->wbb_pbx->setEnabled(true);
            ui->wbb_sld->setEnabled(true);
            ui->wbg_pbx->setEnabled(true);
            ui->wbg_sld->setEnabled(true);
            ui->wbr_pbx->setEnabled(true);
            ui->wbr_sld->setEnabled(true);
            ui->brt_pbx->setEnabled(true);
            ui->brt_sld->setEnabled(true);
            ui->ctt_pbx->setEnabled(true);
            ui->ctt_sld->setEnabled(true);
            ui->gma_pbx->setEnabled(true);
            ui->gma_sld->setEnabled(true);

            ui->epsg_btn->setEnabled(false);
            ui->bglv_btn->setEnabled(true);

            ret = SetQHYCCDParam(camhandle,CONTROL_TRANSFERBIT,8);
            ui->bit_cbx->setCurrentIndex(0);

            ret = SetQHYCCDParam(camhandle,CONTROL_WBB,50);
            ui->wbb_pbx->setValue(50);
            ret = SetQHYCCDParam(camhandle,CONTROL_WBG,50);
            ui->wbg_pbx->setValue(50);
            ret = SetQHYCCDParam(camhandle,CONTROL_WBR,50);
            ui->wbr_pbx->setValue(50);

            ret = SetQHYCCDParam(camhandle,CONTROL_BRIGHTNESS,0);
            ui->brt_pbx->setValue(0);
            ret = SetQHYCCDParam(camhandle,CONTROL_CONTRAST,0);
            ui->ctt_pbx->setValue(0);
            ret = SetQHYCCDParam(camhandle,CONTROL_GAMMA,1.0);
            ui->gma_pbx->setValue(1.0);

            ret = SetQHYCCDDebayerOnOff(camhandle,false);
            ui->clr_cbx->setCurrentText("OFF");
        }else{

        }

        ui->cfwp_btn->setEnabled(true);//使能滤镜轮检查按键
        ui->cfws_btn->setEnabled(true);//使能滤镜轮状态获取按键
        ui->odtc_cbbx->setEnabled(true);//使能滤镜轮控制按键
        ui->gtcn_btn->setEnabled(true);//使能状态获取按键
//        int num = GetQHYCCDParam(camhandle,CONTROL_CFWSLOTSNUM);
//        QStringList cfwList = {"0","1","2","3","4","5","6","7",
//                               "8","9","a","b","c","d","e","f"};
//        for(int i = 0;i < num;i ++){
//            ui->odtc_cbbx->addItem(cfwList[i]);
//        }

    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = InitQHYCCD(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can new a class for camera,this class include parameter and other resource,and you "
                             "only need use this function one time,if you use this function more time,the function will create "
                             "new classes,these classes will make program confuse which class is it need.'camhandle' is getted by "
                             "'OpenQHYCCD()'.\n"
                             "\n"
                             "Function run result:\n"
                             "Initializing fail.");
    }

    EnableQHYCCDMessage(true);

#if 0
    QModelIndex index;//定义下拉列表选项的下标
    QVariant v(0);
    index = ui->isal_cbbx->model()->index(1,1);//设置功能检测下拉选项使能
    ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
    index = ui->isal_cbbx->model()->index(1,0);//设置功能检测下拉选项不使能
    ui->isal_cbbx->model()->setData(index,v,Qt::UserRole - 1);
#endif

    cam.length = GetQHYCCDMemLength(camhandle);
    cam.ImgData = (unsigned char *)malloc(cam.length*2);
    memset(cam.ImgData,0,cam.length*2);
//    usleep(100);
    cam.ImgDataSave = (unsigned char *)malloc(cam.length*2);
    memset(cam.ImgDataSave,0,cam.length*2);
}

/*关闭相机*/
void Guider::Close()
{
    ret = CloseQHYCCD(camhandle);
    if(ret == QHYCCD_SUCCESS){
        if(cam.ImgDataSave != NULL)
            free(cam.ImgDataSave);
        if(cam.ImgData != NULL)
            free(cam.ImgData);

        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = CloseQHYCCD(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can disconnect camera,if successfully,it will return 'QHYCCD_SUCCESS',if error,"
                             "it will return QHYCCD_ERROR.\n"
                             "\n"
                             "Function run result:\n"
                             "Close camera successfully.");
        ui->reles_btn->setEnabled(true);//使能资源释放按键
        /*由于相机被关闭，大部分按键都设置为不使能*/
//        ui->dlfw_btn->setEnabled(false);
#ifdef Q_OS_MAC
        ui->osxf_btn->setEnabled(false);
#endif
        ui->scan_btn->setEnabled(false);
        ui->gtid_btn->setEnabled(false);
        ui->open_btn->setEnabled(false);
        ui->intq_btn->setEnabled(false);
        ui->clos_btn->setEnabled(false);
        ui->fwvs_btn->setEnabled(false);
        ui->mmsp_cbbx->setEnabled(false);
        ui->gtci_btn->setEnabled(false);
        ui->isal_cbbx->setEnabled(false);
        ui->gtpm_cbbx->setEnabled(false);
        ui->eftv_btn->setEnabled(false);
        ui->gtos_btn->setEnabled(false);
        ui->mlth_btn->setEnabled(false);
//        ui->dboo_cbbx->setEnabled(false);
//        ui->stpm_cbbx->setEnabled(false);
        ui->stfm_cbbx->setEnabled(false);
//        ui->stbm_cbbx->setEnabled(false);
//        ui->strl_btn->setEnabled(false);
        ui->epsg_btn->setEnabled(false);
        ui->gtsg_btn->setEnabled(false);
        ui->cerd_btn->setEnabled(false);
//        ui->cexp_btn->setEnabled(false);
        ui->bglv_btn->setEnabled(false);
        ui->gtlv_btn->setEnabled(false);
        ui->stpl_btn->setEnabled(false);
        ui->cfwp_btn->setEnabled(false);
        ui->cfws_btn->setEnabled(false);
        ui->odtc_cbbx->setEnabled(false);
        ui->gtcn_btn->setEnabled(false);
        ui->sttp_btn->setEnabled(false);
        ui->gttp_btn->setEnabled(false);
        ui->spwm_btn->setEnabled(false);
        ui->gpwm_btn->setEnabled(false);
        ui->exp_sld->setEnabled(false);
        ui->exp_pbx->setEnabled(false);
        ui->unt_btn->setEnabled(false);
        ui->clr_cbx->setEnabled(false);
        ui->bit_cbx->setEnabled(false);
        ui->bin_cbx->setEnabled(false);
        ui->stx_pbx->setEnabled(false);
        ui->sty_pbx->setEnabled(false);
        ui->szx_pbx->setEnabled(false);
        ui->szy_pbx->setEnabled(false);
        ui->roi_btn->setEnabled(false);
        ui->gan_pbx->setEnabled(false);
        ui->gan_sld->setEnabled(false);
        ui->oft_pbx->setEnabled(false);
        ui->oft_sld->setEnabled(false);
        ui->spd_pbx->setEnabled(false);
        ui->spd_sld->setEnabled(false);
        ui->tfc_pbx->setEnabled(false);
        ui->tfc_sld->setEnabled(false);
        ui->ddr_cbx->setEnabled(false);
        ui->wbb_pbx->setEnabled(false);
        ui->wbb_sld->setEnabled(false);
        ui->wbg_pbx->setEnabled(false);
        ui->wbg_sld->setEnabled(false);
        ui->wbr_pbx->setEnabled(false);
        ui->wbr_sld->setEnabled(false);
        ui->brt_pbx->setEnabled(false);
        ui->brt_sld->setEnabled(false);
        ui->ctt_pbx->setEnabled(false);
        ui->ctt_sld->setEnabled(false);
        ui->gma_pbx->setEnabled(false);
        ui->gma_sld->setEnabled(false);
        ui->ost_cbx->setEnabled(false);
        ui->ost_pbx->setEnabled(false);
        ui->ost_sld->setEnabled(false);
        ui->singleSaveBtn->setEnabled(false);
        ui->singleFormatCbx->setEnabled(false);
        ui->liveSaveBtn->setEnabled(false);
        ui->liveFormatCbx->setEnabled(false);
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = CloseQHYCCD(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can disconnect camera,if successfully,it will return 'QHYCCD_SUCCESS',if error,"
                             "it will return QHYCCD_ERROR."
                             "\n"
                             "Function run result:\n"
                             "Closing camera fail.");
    }
}

/*释放资源*/
void Guider::ReleaseResource()
{
    ret = ReleaseQHYCCDResource();
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = ReleaseQHYCCDResource();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function will release all resource.If successfully,its return value is QHYCCD_SUCCSESS,if"
                             "error,its return value is QHYCCD_ERROR.\n"
                             "\n"
                             "Function run result:\n"
                             "Release resource successfully.");
        ui->intr_btn->setEnabled(true);
        ui->reles_btn->setEnabled(false);//资源释放后设置资源释放按钮不使能
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "ret = ReleaseQHYCCDResource();\n"
                             "\n"
                             "Introduction:\n"
                             "    This function will release all resource.If successfully,its return value is QHYCCD_SUCCSESS,if"
                             "error,its return value is QHYCCD_ERROR.\n"
                             "\n"
                             "Function run result:\n"
                             "Release resource fail.");
    }
}

/*获取固件版本*/
void Guider::FirmWareVersion()
{
    ret = GetQHYCCDFWVersion(camhandle,fwv);
    if(ret == QHYCCD_SUCCESS){
        if((fwv[0] >> 4) <= 9){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;\n"
                                 "unsigned char fwv[32];\n"
                                 "ret = GetQHYCCDFWVersion(camhandle,fwv);\n"
                                 "if(ret == QHYCCD_SUCCESS)\n"
                                 "{\n"
                                 "    if((fwv[0] >> 4) <= 9)\n"
                                 "    {\n"
                                 "        printf('Version:20%d-%d-%d',fwv[0]>>4+0x10,fwv[0]&~0xf0,fwv[1]);\n"
                                 "    }\n"
                                 "    else\n"
                                 "    {\n"
                                 "        printf('Version:20%d-%d-%d',fwv[0]>>4,fwv[0]&~0xf0,fwv[1]);\n"
                                 "    }\n"
                                 "}\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can get firmware's version,and according to different year,it has different count methods.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Firmware version:20"
                                 + QString::number(((fwv[0] >> 4) + 0x10),16,0)+ "-"
                                 + QString::number((fwv[0]&~0xf0),16,0) + "-"
                                 + QString::number(fwv[1],16,0));
        }
        else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;\n"
                                 "unsigned char fwv[32];\n"
                                 "ret = GetQHYCCDFWVersion(camhandle,fwv);\n"
                                 "if(ret == QHYCCD_SUCCESS)\n"
                                 "{\n"
                                 "    if((fwv[0] >> 4) <= 9)\n"
                                 "    {\n"
                                 "        printf('Version:20%d-%d-%d',fwv[0]>>4+0x10,fwv[0]&~0xf0,fwv[1]);\n"
                                 "    }\n"
                                 "    else\n"
                                 "    {\n"
                                 "        printf('Version:20%d-%d-%d',fwv[0]>>4,fwv[0]&~0xf0,fwv[1]);\n"
                                 "    }\n"
                                 "}\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can get firmware's version,and according to different year,it has different count methods.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Firmware version:20"
                                 + QString::number((fwv[0] >> 4),16,0)+ "-"
                                 + QString::number((fwv[0]&~0xf0),16,0) + "-"
                                 + QString::number(fwv[1],16,0));
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "unsigned char fwv[32];\n"
                             "ret = GetQHYCCDFWVersion(camhandle,fwv);\n"
                             "if(ret == QHYCCD_SUCCESS)\n"
                             "{\n"
                             "    if((fwv[0] >> 4) <= 9)\n"
                             "    {\n"
                             "        printf('Version:20%d-%d-%d',fwv[0]>>4+0x10,fwv[0]&~0xf0,fwv[1]);\n"
                             "    }\n"
                             "    else\n"
                             "    {\n"
                             "        printf('Version:20%d-%d-%d',fwv[0]>>4,fwv[0]&~0xf0,fwv[1]);\n"
                             "    }\n"
                             "}\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get firmware's version,and according to different year,it has different count method.\n"
                             "\n"
                             "Function run result:\n"
                             "Get firmware version failed.");
    }
}

/*最大最小值及步长获取函数*/
void Guider::MinMaxStep()
{
    double min = 0,max = 0,step = 0;
    int order = 0;
    int i = ui->mmsp_cbbx->currentIndex();//获取当前选中的下拉选项的下标
    QString list[] = {"CONTROL_OFFSET",
                      "CONTROL_EXPOSURE",
                      "CONTROL_GAIN",
                      "CONTROL_SPEED",
                      "CONTROL_CURPWM"
                      "CONTROL_MANUPWM"
                      "CONTROL_CURTEMP"
                      "CONTROL_COOLER",
                      "CONTROL_USBTRAFFIC",
                      "CONTROL_TRANSFERBIT",
                      "CONTROL_WBR",
                      "CONTROL_WBB",
                      "CONTROL_WBG",
                      "CONTROL_GAMMA",
                      "CONTROL_BRIGHTNESS",
                      "CONTROL_CONTRAST",
                      "CONTROL_CFWSLOTNUM"
                      "CONTROL_AMPV",
                      "CAM_LIGHT_PERFORMANCE_MODE",
                      "CONTROL_DDR"};

    for(;order < 20; order ++){
        if(list[i-1].compare(idList[order]) == 0){
            break;
        }
    }

    if(i == 0){
        ui->opt_tbs->clear();
    }else{
        ret = IsQHYCCDControlAvailable(camhandle,(enum CONTROL_ID)order);
        if(ret == QHYCCD_SUCCESS){
            ret = GetQHYCCDParamMinMaxStep(camhandle,(enum CONTROL_ID)order,&min,&max,&step);
            if(ret == QHYCCD_SUCCESS){
                ui->opt_tbs->clear();
                ui->opt_tbs->setText("Introduction:\n"
                                     "    Get parameter min,max and step success,"
                                     "min = " + QString::number(min,16,1) + ","
                                     "max = " + QString::number(max,16,1) + ","
                                     "step = " + QString::number(step,16,1) + "."
                                     "You can get them by 'ret = GetQHYCCDParamMinMaxStep(camhandle," +
                                     list[i - 1] + ",&min,&max,&step);'.");
            }else{
                ui->opt_tbs->setText("Get parameter min,max and step success.");
            }
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    This camera doesn't support this function,you can check if camera support a function "
                                 "by 'IsQHYCCDControlAvailable'.");
        }
    }
}

/*获取相机的片上信息*/
void Guider::GetChipInfo()
{
    ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);
    if(ret == QHYCCD_SUCCESS){
//        cam.chipw = chipw;
//        cam.chiph = chiph;
//        cam.pixelw = pixelw;
//        cam.pixelh = pixelh;
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "uint32_t imagew,imageh,bpp;\n"
                             "double chipw,chiph,pixelw,pixelh;\n"
                             "ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);\n"
                             "\n"
                             "Function run result:\n"
                             "    This function can get camera's chip info,main is hardware parameter,you can use imagew,imageh and bpp be other function's parameter.\n"
                             "\n"
                             "Function run result:\n"
                             "GetQHYCCDChipInfo success!\n"
                             "Chip height: " + QString::number(chiph,16,2)  + " mm\n" +
                             "Chip width: " + QString::number(chipw,16,2)  + " mm\n" +
                             "Chip pixel width: " + QString::number(pixelw,16,1) + " um\n" +
                             "Chip pixel height: " + QString::number(pixelh,16,1) + " um\n" +
                             "Image width: " + QString::number(imagew,16,0) + " \n" +
                             "Image height: " + QString::number(imageh,16,0) + " \n" +
                             "Camera depth: " + QString::number(bpp,16,0));
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;\n"
                             "uint32_t imagew,imageh,bpp;\n"
                             "double chipw,chiph,pixelw,pixelh;\n"
                             "ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);\n"
                             "\n"
                             "Function run result:\n"
                             "    This function can get camera's chip info,main is hardware parameter,you can use imagew,imageh and bpp be other function's parameter.\n"
                             "\n"
                             "Function run result:\n"
                             "Get QHYCCD chip information failed.");
    }
}

/*获取图像数据长度*/
void Guider::MemLength()
{
    length = GetQHYCCDMemLength(camhandle);
    if(length > 0){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int length = 0;"
                             "length = GetQHYCCDMemLength(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's memory length,you can new enough space for image data by this length.\n"
                             "\n"
                             "Function run result:\n"
                             "Memlength:" + QString::number(length,16,0));
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int length = 0;"
                             "length = GetQHYCCDMemLength(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's memory length,you can new enough space for image data by this length.\n"
                             "\n"
                             "Function run result:\n"
                             "Get memory length failed.");
    }
}

/*获取参数数值，实现方法与MinMaxStep函数类似*/
void Guider::GetParam()
{
    int order = 0,ret = QHYCCD_SUCCESS;
    int i = ui->gtpm_cbbx->currentIndex();
    double value = 0;

    QString list[] = {"CONTROL_WBR",
                      "CONTROL_WBG",
                      "CONTROL_WBB",
                      "CONTROL_EXPOSURE",
                      "CONTROL_GAIN",
                      "CONTROL_OFFSET",
                      "CONTROL_SPEED",
                      "CONTROL_USBTRAFFIC",
                      "CONTROL_TRANSFERBIT",
                      "CONTROL_CURTEMP",
                      "CONTROL_CURPWM",
                      "CONTROL_COOLER",
                      "CONTROL_BRIGHTNESS",
                      "CONTROL_CONTRAST",
                      "CONTROL_GAMMA",
                      "CONTROL_AMPV",
                      "CONTROL_VCAM",
                      "CAM_CHIPTEMPERATURESENSOR_INTERFACE",
                      "CAM_VIEW_MODE",
                      "CAM_GPS",
                      "CONTROL_CFWSLOTSNUM",
                      "CONTROL_CFWPORT",
                      "CONTROL_DDR",
                      "CAM_LIGHT_PERFORMANCE_MODE",
                      "CAM_QHY5II_GUIDE_MODE",
                      "DDR_BUFFER_CAPACITY",
                      "DDR_BUFFER_READ_THRESHOLD"};

    for(;order < 27;order ++){
        if(list[i - 1].compare(idList[order]) == 0){
            break;
        }
    }

    if(i == 0){
        ui->opt_tbs->clear();
    }else{
        ret = IsQHYCCDControlAvailable(camhandle,(enum CONTROL_ID)order);
        if(ret == QHYCCD_SUCCESS){
            value = GetQHYCCDParam(camhandle,(enum CONTROL_ID)order);
            if(value != QHYCCD_ERROR){
                ui->opt_tbs->clear();
                ui->opt_tbs->setText("Introduction:\n"
                                     "    Get parameter value success,the value is " + QString::number(value,10,1) + "."
                                     "You can get camera parameter value by 'value = GetQHYCCDParam(camhandle," +
                                     list[i - 1] + ");'.");
            }else{
                ui->opt_tbs->clear();
                ui->opt_tbs->setText("Introduction:\n"
                                     "    Get parameter value fail,the value is " + QString::number(value,10,1) + "."
                                     "You can get camera parameter value by 'value = GetQHYCCDParam(camhandle," +
                                     list[i - 1] + ");'.");
            }
        }else{
            ui->opt_tbs->setText("Introduction:\n"
                                 "    This camera doesn't support this function,you can check if camera support a function "
                                 "by 'IsQHYCCDControlAvailable'.");
        }
    }
}

/*获取有效区域*/
void Guider::GetEffectiveArea()
{
    ret = GetQHYCCDEffectiveArea(camhandle,&estartx,&estarty,&esizex,&esizey);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;"
                             "uint32_t estartx,estarty,esizex,esizey;\n"
                             "ret = GetQHYCCDEffectiveArea(camhandle,&estartx,&estarty,&esizex,&esizey);"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's effective area.\n"
                             "\n"
                             "Function run result:\n"
                             "Effective area startx:" + QString::number(estartx,16,0) + "\n"
                             "Effective area starty:" + QString::number(estarty,16,0) + "\n"
                             "Effective area sizex:" + QString::number(esizex,16,0)   + "\n"
                             "Effective area sizey:" + QString::number(esizey,16,0));
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;"
                             "uint32_t estartx,estarty,esizex,esizey;\n"
                             "ret = GetQHYCCDEffectiveArea(camhandle,&estartx,&estarty,&esizex,&esizey);"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's effective area.\n"
                             "\n"
                             "Function run result:\n"
                             "Get effictive area failed.");
    }
}

/*获取过扫描区域*/
void Guider::GetOverscanArea()
{
    ret = GetQHYCCDOverScanArea(camhandle,&ostartx,&ostarty,&osizex,&osizey);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;"
                             "uint32_t ostartx,ostarty,osizex,osizey;\n"
                             "ret = GetQHYCCDEffectiveArea(camhandle,&ostartx,&ostarty,&osizex,&osizey);"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's overscan area.\n"
                             "\n"
                             "Function run result:\n"
                             "Overscan area startx:" + QString::number(ostartx,16,0) + "\n"
                             "Overscan area starty:" + QString::number(ostarty,16,0) + "\n"
                             "Overscan area sizex:" + QString::number(osizex,16,0)   + "\n"
                             "Overscan area sizey:" + QString::number(osizey,16,0));
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_ERROR;"
                             "uint32_t ostartx,ostarty,osizex,osizey;\n"
                             "ret = GetQHYCCDEffectiveArea(camhandle,&ostartx,&ostarty,&osizex,&osizey);"
                             "\n"
                             "Introduction:\n"
                             "    This function can get camera's overscan area.\n"
                             "\n"
                             "Function run result:\n"
                             "Get effictive area failed.");
    }
}

/*设置彩色相机开关*/
void Guider::SetDebayerOnOff()
{
    if(ui->clr_cbx->currentText() == "ON"){
        ret = SetQHYCCDDebayerOnOff(camhandle,true);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDDebayerOnOff(camhandle,true);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera color mode on or off,only color camera can use this function.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set camera debayer on successfully.");
            isCvtColor = true;
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDDebayerOnOff(camhandle,true);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera color mode on or off,only color camera can use this function.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set camera debayer on failed.");
            isCvtColor = false;
        }
    }else{
        ret = SetQHYCCDDebayerOnOff(camhandle,false);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDDebayerOnOff(camhandle,false);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera color mode on or off,only color camera can use this function.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set camera debayer off successfully.");
            isCvtColor = false;
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDDebayerOnOff(camhandle,false);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera color mode on or off,only color camera can use this function.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set camera debayer off failed.");
            isCvtColor = false;
        }
    }
}

/*设置相机的工作模式*/
void Guider::SetStreamMode()
{
    int i = ui->stfm_cbbx->currentIndex();
    switch(i){
    case 0:
        ui->opt_tbs->clear();
        //ui->opt_tbs->setText("Output Information\nShow Area");
        break;
    case 1:
        ret = SetQHYCCDStreamMode(camhandle,0);
        if(ret == QHYCCD_SUCCESS){
            ui->stfm_cbbx->setEnabled(false);
            ui->intq_btn->setEnabled(true);
            flag_single = true;
            flag_live   = false;
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;"
                                 "ret = SetQHYCCDStreamMode(camhandle,0);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's frame mode(single or live),set to be 0 is single mode,set to be 1 is live mode."
                                 "If Successfully,its return value is QHYCCD_SUCCESS,if error,its return value is QHYCCD_ERROR.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set single frame mode successfully.Please click 'InitQHYCCD' button to do next operation.");
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;"
                                 "ret = SetQHYCCDStreamMode(camhandle,0);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's frame mode(single or live),set to be 0 is single mode,set to be 1 is live mode."
                                 "If Successfully,its return value is QHYCCD_SUCCESS,if error,its return value is QHYCCD_ERROR.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set single frame mode fail.");
        }
        break;
    case 2:
        ret = SetQHYCCDStreamMode(camhandle,1);
        if(ret == QHYCCD_SUCCESS){
            ui->stfm_cbbx->setEnabled(false);
            ui->intq_btn->setEnabled(true);
            flag_single = false;
            flag_live   = true;
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;"
                                 "ret = SetQHYCCDStreamMode(camhandle,1);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's frame mode(single or live),set to be 0 is single mode,set to be 1 is live mode."
                                 "If Successfully,its return value is QHYCCD_SUCCESS,if error,its return value is QHYCCD_ERROR.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set single frame mode successfully.Please click 'InitQHYCCD' button to do next operation.");
        }else {
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_ERROR;"
                                 "ret = SetQHYCCDStreamMode(camhandle,1);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's frame mode(single or live),set to be 0 is single mode,set to be 1 is live mode."
                                 "If Successfully,its return value is QHYCCD_SUCCESS,if error,its return value is QHYCCD_ERROR.\n"
                                 "\n"
                                 "Function run result:\n"
                                 "Set single frame mode fail.");
        }
        break;
    default:
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Some error occered.");
        break;
    }
}

/*双精度类型数据转整形数据的函数，用来避免类型转换时精度差异带来的数据丢失*/
int doubleToInt(double d)
{
    double intPart = floor(d);//向下取整
    if ((d - intPart) >= (double)0.5){
        return (intPart + 1);
    }else{
        return intPart;
    }
}

/*设置SpinBox的值随滚动条变化而变化，用来保持两者值同步*/
//void Guider::SpinBoxShowNum()
//{
//    int index = ui->stpm_cbbx->currentIndex();
//    if(index == 17 || index == 18 || index == 19 || index == 21){
//        int i  = sdlg->ui->sp_valu_sld->value();
//        double d = (double)i / 10;
//        sdlg->ui->sp_valu_spb->setValue(d);
//    }else{
//        double d = (double)sdlg->ui->sp_valu_sld->value();
//        sdlg->ui->sp_valu_spb->setValue(d);
//    }
//}

/*设置滚动条的值随SpinBox变化而变化，用来保持两者值同步*/
//void Guider::SliderShowNum()
//{
//    int index = ui->stpm_cbbx->currentIndex();
//    if(index == 17 || index == 18 || index == 19 || index == 21){
//        double d = sdlg->ui->sp_valu_spb->value();
//        d = d * 10;
//        sdlg->ui->sp_valu_sld->setValue(doubleToInt(d));
//    }else{
//        int d = (int)sdlg->ui->sp_valu_spb->value();
//        sdlg->ui->sp_valu_sld->setValue(d);
//    }
//}

/*用来设置曝光设置下，单位及SpinBox、滚动条的最大最小值和步长*/
//void Guider::SPUnitBtnClicked()
//{
//    QString t = sdlg->ui->sp_unit_btn->text();
//    QString u = "ms";
//    if(t.toStdString().data() == u){
//        sdlg->ui->sp_unit_btn->setText("s");
//        if(exposure_max == 0){
//            sdlg->ui->sp_valu_sld->setMaximum(1000);//设置滚动条上限为1500
//            sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限为0
//            sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//            sdlg->ui->sp_valu_spb->setMaximum(1000);//设置SpinBox上限为1500
//            sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(exposure_max / 1000000);//设置滚动条上限
//            sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限为0
//            sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//            sdlg->ui->sp_valu_spb->setMaximum(exposure_max / 1000000);//设置SpinBox上限
//            sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }
//    }else{
//        sdlg->ui->sp_unit_btn->setText("ms");
//        sdlg->ui->sp_valu_sld->setMaximum(10000);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限为0
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//        sdlg->ui->sp_valu_spb->setMaximum(10000);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//    }
//}

/*点击对话框的OK按键进行参数设置*/
//void Guider::SPOKBtnClicked()
//{
//    int order = 0;
//    int i = ui->stpm_cbbx->currentIndex();
//    QString u = "ms";//用来判断曝光时间设置的时间单位
//    QString t = sdlg->ui->sp_unit_btn->text();//获取时间单位按钮的当前单位
//    double v = sdlg->ui->sp_valu_spb->value();//获取要设置的参数值

//    QString list[] = {"CONTROL_WBR",
//                      "CONTROL_WBG",
//                      "CONTROL_WBB",
//                      "CONTROL_EXPOSURE",
//                      "CONTROL_GAIN",
//                      "CONTROL_OFFSET",
//                      "CONTROL_SPEED",
//                      "CONTROL_USBTRAFFIC",
//                      "CONTROL_TRANSFERBIT",
//                      "CONTROL_ROWNOISERE",
//                      "CONTROL_MANULPWM",
//                      "CAM_GPS",
//                      "CAM_IGNOREOVERSCAN_INTERFACE",
//                      "QHYCCD_3A_AUTOBALANCE",
//                      "QHYCCD_3A_AUTOEXPOSURE",
//                      "QHYCCD_3A_AUTOFOCUS",
//                      "CONTROL_BRIGHTNESS",
//                      "CONTROL_CONTRAST",
//                      "CONTROL_GAMMA",
//                      "CONTROL_AMPV",
//                      "CONTROL_COOLER",
//                      "CONTROL_VCAM",
//                      "CAM_VIEW_MODE",
//                      "CONTROL_CFWPORT",
//                      "CONTROL_DDR",
//                      "CAM_LIGHT_PERFORMANCE_MODE",
//                      "CAM_QHY5II_GUIDE_MODE"};

//    for(; order < 53;order ++){
//        if(list[i-1].compare(idList[order]) == 0){
//            break;
//        }
//    }

//    if(i == 0){
//        ui->opt_tbs->clear();
//    }else{
//        ret = IsQHYCCDControlAvailable(camhandle,(enum CONTROL_ID)order);
//        if(ret == QHYCCD_SUCCESS){
//            if(order == 8){
//                if(sdlg->ui->sp_unit_btn->text() == "ms")
//                    v = v * 1000;
//                else
//                    v = v * 1000 * 1000;
//            }

//            ret = SetQHYCCDParam(camhandle,(enum CONTROL_ID)order,v);
//            if(ret == QHYCCD_SUCCESS){
//                ui->opt_tbs->setText("Introduction:\n"
//                                     "    Set parameter success,you can set parameter by 'ret = SetQHYCCDParam(camhandle," +
//                                     list[i-1] + ");'.");
//            }else{
//                ui->opt_tbs->setText("Introduction:\n"
//                                     "    Set parameter fail,you can set parameter by 'ret = SetQHYCCDParam(camhandle," +
//                                     list[i-1] + ");'.");
//            }
//        }else{
//            ui->opt_tbs->setText("Introduction:\n"
//                                 "    This camera doesn't support this function,you can check if camera support a function "
//                                 "by 'IsQHYCCDControlAvailable'.");
//        }
//    }
//}

/* SetParam按键对应的函数，通过点击选项弹出的对话框，设置各项参数的最大最小值及步长*/
/* 因为SetParam按键的可选项是由Init按键来决定使能与否的，所以这里只是设置最大最小值及步长，
 * 即使GetQHYCCDMinMaxStep函数执行失败也会使用默认参数进行设置，不过有些参数比较特殊，
 * 因此不会进行容错判断
 */
//void Guider::SetParam()
//{
//    int i = ui->stpm_cbbx->currentIndex();
//    sdlg->ui->sp_unit_btn->setVisible(false);
//    sdlg->ui->sp_valu_spb->setFocusPolicy(Qt::NoFocus);
////    sdlg->ui->sp_valu_spb->setReadOnly(true);

//    switch(i){
//    case 0:
//        ui->opt_tbs->clear();
//        //ui->opt_tbs->setText("Output Information\nShow Area");
//        break;
//    case 1:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("WBR");
//        sdlg->ui->sp_valu_spb->setDecimals(0);//设置SpinBox的精度
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_WBR,&wbr_min,&wbr_max,&wbr_step);//设置之前先用函数获取最大最小值及步长
//        if(ret == QHYCCD_SUCCESS){
//            if(wbr_max != wbr_min){//最大最小值不同时才会使用获取到的参数进行设置，否则使用默认设置
//                sdlg->ui->sp_valu_sld->setMaximum(wbr_max);
//                sdlg->ui->sp_valu_sld->setMinimum(wbr_min);
//                sdlg->ui->sp_valu_sld->setPageStep(wbr_step);

//                sdlg->ui->sp_valu_spb->setMaximum(wbr_max);
//                sdlg->ui->sp_valu_spb->setMinimum(wbr_min);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(255);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(255);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(255);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(255);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }
//        break;
//    case 2:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("WBG");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_WBG,&wbg_min,&wbg_max,&wbg_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(wbg_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(wbg_max);
//                sdlg->ui->sp_valu_sld->setMinimum(wbg_min);
//                sdlg->ui->sp_valu_sld->setPageStep(wbg_step);

//                sdlg->ui->sp_valu_spb->setMaximum(wbg_max);
//                sdlg->ui->sp_valu_spb->setMinimum(wbg_min);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(255);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(255);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(255);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(255);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }
//        break;
//    case 3:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("WBB");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_WBB,&wbb_min,&wbb_max,&wbb_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(wbb_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(wbb_max);
//                sdlg->ui->sp_valu_sld->setMinimum(wbb_min);
//                sdlg->ui->sp_valu_sld->setPageStep(wbb_step);

//                sdlg->ui->sp_valu_spb->setMaximum(wbb_max);
//                sdlg->ui->sp_valu_spb->setMinimum(wbb_min);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(255);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(255);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(255);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(255);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }

//        break;
//    case 4:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("EXPOSURE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_EXPOSURE,&exposure_min,&exposure_max,&exposure_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(exposure_max == 0){
//                sdlg->ui->sp_unit_btn->setVisible(true);//设置按键可视
//                sdlg->ui->sp_unit_btn->setText("s");//设置单位为秒

//                sdlg->ui->sp_valu_sld->setMaximum(1000);//设置滚动条上限为1500
//                sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限为0
//                sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//                sdlg->ui->sp_valu_spb->setMaximum(1000);//设置SpinBox上限为1500
//                sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//                sdlg->ui->sp_valu_spb->setSingleStep(1);
//            }else{
//                sdlg->ui->sp_unit_btn->setVisible(true);//设置按键可视
//                sdlg->ui->sp_unit_btn->setText("s");//设置单位为秒

//                sdlg->ui->sp_valu_sld->setMaximum(exposure_max / 1000000);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//                sdlg->ui->sp_valu_spb->setMaximum(exposure_max / 1000000);//设置SpinBox上限为1500
//                sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//                sdlg->ui->sp_valu_spb->setSingleStep(1);
//            }
//        }else{
//            sdlg->ui->sp_unit_btn->setVisible(true);//设置按键可视
//            sdlg->ui->sp_unit_btn->setText("s");//设置单位为秒

//            sdlg->ui->sp_valu_sld->setMaximum(1000);//设置滚动条上限为1500
//            sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限为0
//            sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条步长为1

//            sdlg->ui->sp_valu_spb->setMaximum(1000);//设置SpinBox上限为1500
//            sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限为0
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }
//        break;
//    case 5:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("GAIN");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_GAIN,&gain_min,&gain_max,&gain_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(gain_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(gain_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(gain_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(gain_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(gain_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(gain_min);//设置SpinBox下限
//                sdlg->ui->sp_valu_spb->setSingleStep(gain_step);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(60);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(60);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//                sdlg->ui->sp_valu_spb->setSingleStep(1);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(60);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(60);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }
//        break;
//    case 6:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("OFFSET");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_OFFSET,&offset_min,&offset_max,&offset_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(offset_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(offset_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(offset_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(offset_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(offset_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(offset_min);//设置SpinBox下限
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(255);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(255);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(255);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(255);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }
//        break;
//    case 7:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("SPEED");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_SPEED,&speed_min,&speed_max,&speed_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(speed_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(speed_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(speed_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(speed_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(speed_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(speed_min);//设置SpinBox下限
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(2);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(2);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(2);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(2);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }
//        break;
//    case 8:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("TRAFFIC");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_USBTRAFFIC,&traffic_min,&traffic_max,&traffic_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(traffic_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(traffic_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(traffic_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(traffic_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(traffic_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(traffic_min);//设置SpinBox下限
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(60);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(60);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(60);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(60);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//        }
//        break;
//    case 9:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("TRANSFERBIT");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_TRANSFERBIT,&transferbit_min,&transferbit_max,&transferbit_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(transferbit_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(transferbit_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(transferbit_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(transferbit_step);//设置滚动条点击步长
//                sdlg->ui->sp_valu_sld->setSingleStep(transferbit_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(transferbit_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(transferbit_min);//设置SpinBox下限
//                sdlg->ui->sp_valu_spb->setSingleStep(transferbit_step);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(16);
//                sdlg->ui->sp_valu_sld->setMinimum(8);
//                sdlg->ui->sp_valu_sld->setPageStep(8);
//                sdlg->ui->sp_valu_sld->setSingleStep(8);

//                sdlg->ui->sp_valu_spb->setMaximum(16);
//                sdlg->ui->sp_valu_spb->setMinimum(8);
//                sdlg->ui->sp_valu_spb->setSingleStep(8);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(16);
//            sdlg->ui->sp_valu_sld->setMinimum(8);
//            sdlg->ui->sp_valu_sld->setPageStep(8);
//            sdlg->ui->sp_valu_sld->setSingleStep(8);

//            sdlg->ui->sp_valu_spb->setMaximum(16);
//            sdlg->ui->sp_valu_spb->setMinimum(8);
//            sdlg->ui->sp_valu_spb->setSingleStep(8);
//        }
//        break;
//    case 10:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("ROWNOISERE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 11:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("PWM");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_MANULPWM,&manupwm_min,&manupwm_max,&manupwm_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(manupwm_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(manupwm_max);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(manupwm_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(manupwm_step);//设置滚动条点击步长
//                sdlg->ui->sp_valu_sld->setSingleStep(manupwm_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(manupwm_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(manupwm_min);//设置SpinBox下限
//                sdlg->ui->sp_valu_spb->setSingleStep(manupwm_step);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(255);
//                sdlg->ui->sp_valu_sld->setMinimum(0);
//                sdlg->ui->sp_valu_sld->setPageStep(1);
//                sdlg->ui->sp_valu_sld->setSingleStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(255);
//                sdlg->ui->sp_valu_spb->setMinimum(0);
//                sdlg->ui->sp_valu_spb->setSingleStep(1);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(255);
//            sdlg->ui->sp_valu_sld->setMinimum(0);
//            sdlg->ui->sp_valu_sld->setPageStep(1);
//            sdlg->ui->sp_valu_sld->setSingleStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(255);
//            sdlg->ui->sp_valu_spb->setMinimum(0);
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }
//        break;
//    case 12:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("GPS");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 13:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("IGNOREOVERSCAN");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 14:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("AUTOBALANCE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 15:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("AUTOEXPOSURE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(9);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(9);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 16:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("AUTOFOCUS");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 17:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("BRIGHTNESS");
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_BRIGHTNESS,&brightness_min,&brightness_max,&brightness_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(brightness_max != brightness_min){
//                sdlg->ui->sp_valu_spb->setDecimals(1);

//                sdlg->ui->sp_valu_sld->setMaximum(brightness_max * 10);
//                sdlg->ui->sp_valu_sld->setMinimum(brightness_min * 10);
//                sdlg->ui->sp_valu_sld->setPageStep(brightness_step * 10);

//                sdlg->ui->sp_valu_spb->setMaximum(brightness_max);
//                sdlg->ui->sp_valu_spb->setMinimum(brightness_min);
//                sdlg->ui->sp_valu_spb->setSingleStep(brightness_step);
//            }else{
//                sdlg->ui->sp_valu_spb->setDecimals(1);

//                sdlg->ui->sp_valu_sld->setMaximum(10);
//                sdlg->ui->sp_valu_sld->setMinimum(-10);
//                sdlg->ui->sp_valu_sld->setPageStep(1);

//                sdlg->ui->sp_valu_spb->setMaximum(1);
//                sdlg->ui->sp_valu_spb->setMinimum(-1);
//                sdlg->ui->sp_valu_spb->setSingleStep(0.1);
//            }
//        }else{
//            sdlg->ui->sp_valu_spb->setDecimals(1);

//            sdlg->ui->sp_valu_sld->setMaximum(10);
//            sdlg->ui->sp_valu_sld->setMinimum(-10);
//            sdlg->ui->sp_valu_sld->setPageStep(1);

//            sdlg->ui->sp_valu_spb->setMaximum(1);
//            sdlg->ui->sp_valu_spb->setMinimum(-1);
//            sdlg->ui->sp_valu_spb->setSingleStep(0.1);
//        }
//        break;
//    case 18:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("CONTRAST");
//        sdlg->ui->sp_valu_spb->setDecimals(1);

//        sdlg->ui->sp_valu_sld->setMaximum(10);
//        sdlg->ui->sp_valu_sld->setMinimum(-10);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(1);
//        sdlg->ui->sp_valu_spb->setMinimum(-1);
//        sdlg->ui->sp_valu_spb->setSingleStep(0.1);
//        break;
//    case 19:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("GAMMA");
//        sdlg->ui->sp_valu_spb->setDecimals(1);

//        sdlg->ui->sp_valu_sld->setMaximum(20);
//        sdlg->ui->sp_valu_sld->setMinimum(0);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(2);
//        sdlg->ui->sp_valu_spb->setMinimum(0);
//        sdlg->ui->sp_valu_spb->setSingleStep(0.1);
//        break;
//    case 20:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("AMPV");
//        sdlg->ui->sp_valu_spb->setDecimals(0);

//        sdlg->ui->sp_valu_sld->setMaximum(2);
//        sdlg->ui->sp_valu_sld->setMinimum(0);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(2);
//        sdlg->ui->sp_valu_spb->setMinimum(0);
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 21:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("COOLER");
//        sdlg->ui->sp_valu_spb->setDecimals(1);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_COOLER,&cooler_min,&cooler_max,&cooler_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(cooler_max != cooler_min || cooler_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(cooler_max * 10);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(cooler_min * 10);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(cooler_step * 10);//设置滚动条点击步长
//                sdlg->ui->sp_valu_sld->setSingleStep(cooler_step * 10);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(cooler_max);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(cooler_min);//设置SpinBox下限
//                sdlg->ui->sp_valu_spb->setSingleStep(cooler_step);
//            }else{
//                sdlg->ui->sp_valu_sld->setMaximum(500);
//                sdlg->ui->sp_valu_sld->setMinimum(-500);
//                sdlg->ui->sp_valu_sld->setPageStep(5);
//                sdlg->ui->sp_valu_sld->setSingleStep(5);

//                sdlg->ui->sp_valu_spb->setMaximum(50);
//                sdlg->ui->sp_valu_spb->setMinimum(-50);
//                sdlg->ui->sp_valu_spb->setSingleStep(0.5);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(500);
//            sdlg->ui->sp_valu_sld->setMinimum(-500);
//            sdlg->ui->sp_valu_sld->setPageStep(5);
//            sdlg->ui->sp_valu_sld->setSingleStep(5);

//            sdlg->ui->sp_valu_spb->setMaximum(50);
//            sdlg->ui->sp_valu_spb->setMinimum(-50);
//            sdlg->ui->sp_valu_spb->setSingleStep(0.5);
//        }
//        break;
//    case 22:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("VCAM");
//        sdlg->ui->sp_valu_spb->setDecimals(0);

//        sdlg->ui->sp_valu_sld->setMaximum(2);
//        sdlg->ui->sp_valu_sld->setMinimum(0);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(2);
//        sdlg->ui->sp_valu_spb->setMinimum(0);
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 23:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("VIEW MODE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);

//        sdlg->ui->sp_valu_sld->setMaximum(2);
//        sdlg->ui->sp_valu_sld->setMinimum(0);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(2);
//        sdlg->ui->sp_valu_spb->setMinimum(0);
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 24:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("CFWPORT");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        ret = GetQHYCCDParamMinMaxStep(camhandle,CONTROL_CFWSLOTSNUM,&cfwslotnum_min,&cfwslotnum_max,&cfwslotnum_step);
//        if(ret == QHYCCD_SUCCESS){
//            if(cfwslotnum_max != cfwslotnum_min || cfwslotnum_max != 0){
//                sdlg->ui->sp_valu_sld->setMaximum(cfwslotnum_max - 1);//设置滚动条上限
//                sdlg->ui->sp_valu_sld->setMinimum(cfwslotnum_min);//设置滚动条下限
//                sdlg->ui->sp_valu_sld->setPageStep(cfwslotnum_step);//设置滚动条点击步长
//                sdlg->ui->sp_valu_sld->setSingleStep(cfwslotnum_step);//设置滚动条点击步长

//                sdlg->ui->sp_valu_spb->setMaximum(cfwslotnum_max - 1);//设置SpinBox上限
//                sdlg->ui->sp_valu_spb->setMinimum(cfwslotnum_min);//设置SpinBox下限
//                sdlg->ui->sp_valu_spb->setSingleStep(cfwslotnum_step);
//            }
//        }else{
//            sdlg->ui->sp_valu_sld->setMaximum(8);//设置滚动条上限
//            sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//            sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//            sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//            sdlg->ui->sp_valu_spb->setMaximum(8);//设置SpinBox上限
//            sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//            sdlg->ui->sp_valu_spb->setSingleStep(1);
//        }
//        break;
//    case 25:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("DDR");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 26:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("LIGHT PERFORMANCE\nMODE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);

//        sdlg->ui->sp_valu_sld->setMaximum(2);
//        sdlg->ui->sp_valu_sld->setMinimum(0);
//        sdlg->ui->sp_valu_sld->setPageStep(1);

//        sdlg->ui->sp_valu_spb->setMaximum(2);
//        sdlg->ui->sp_valu_spb->setMinimum(0);
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    case 27:
//        sdlg->show();
//        sdlg->ui->sp_para_lbl->setText("QHY5II GUIDE\nMODE");
//        sdlg->ui->sp_valu_spb->setDecimals(0);
//        sdlg->ui->sp_valu_sld->setMaximum(1);//设置滚动条上限
//        sdlg->ui->sp_valu_sld->setMinimum(0);//设置滚动条下限
//        sdlg->ui->sp_valu_sld->setPageStep(1);//设置滚动条点击步长
//        sdlg->ui->sp_valu_sld->setSingleStep(1);//设置滚动条点击步长

//        sdlg->ui->sp_valu_spb->setMaximum(1);//设置SpinBox上限
//        sdlg->ui->sp_valu_spb->setMinimum(0);//设置SpinBox下限
//        sdlg->ui->sp_valu_spb->setSingleStep(1);
//        break;
//    default:
//        ui->opt_tbs->clear();
//        ui->opt_tbs->setText("Some error occered.");
//        break;
//    }
//}

/*设置相机的bin mode*/
//void Guider::SetBinMode()
//{
//    int index = ui->stbm_cbbx->currentIndex();
//    switch(index){
//    case 0:
//        ui->opt_tbs->clear();
//        ui->opt_tbs->setText("Output Information\nArea");
//        break;
//    case 1:
//        wbin = 1;
//        hbin = 1;
//        ret = SetQHYCCDBinMode(camhandle,wbin,hbin);
//        if(ret == QHYCCD_SUCCESS){
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 1,hbin = 1;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 1X1 bin mode successfully.");
//        }else{
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 1,hbin = 1;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 1X1 bin mode failed.");
//        }
//        break;
//    case 2:
//        wbin = 2;
//        hbin = 2;
//        ret = SetQHYCCDBinMode(camhandle,wbin,hbin);
//        if(ret == QHYCCD_SUCCESS){
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 2,hbin = 2;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 2X2 bin mode successfully.");
//        }else{
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 2,hbin = 2;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 2X2 bin mode failed.");
//        }
//        break;
//    case 3:
//        wbin = 3;
//        hbin = 3;
//        ret = SetQHYCCDBinMode(camhandle,wbin,hbin);
//        if(ret == QHYCCD_SUCCESS){
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 3,hbin = 3;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 3X3 bin mode successfully.");
//        }else{
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 3,hbin = 3;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 3X3 bin mode failed.");
//        }
//        break;
//    case 4:
//        wbin = 4;
//        hbin = 4;
//        ret = SetQHYCCDBinMode(camhandle,wbin,hbin);
//        if(ret == QHYCCD_SUCCESS){
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 4,hbin = 4;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 4X4 bin mode successfully.");
//        }else{
//            ui->opt_tbs->clear();
//            ui->opt_tbs->setText("Using method:\n"
//                                 "int ret = QHYCCD_SUCCESS;\n"
//                                 "int wbin = 4,hbin = 4;\n"
//                                 "ret = SetQHYCCDBinMode(camhandle,wbin,hbin);\n"
//                                 "\n"
//                                 "Introduction:\n"
//                                 "    This function can set camera's bin mode,and you need use this function with 'SetQHYCCDResolution()',bin mode setting "
//                                 "only take effect after you run 'SetQHYCCDResolution()'."
//                                 "\n"
//                                 "Function run result:\n"
//                                 "Set 4X4 bin mode failed.");
//        }
//        break;
//    default:
//        ui->opt_tbs->clear();
//        ui->opt_tbs->setText("Some error eccered.");
//        break;
//    }

//    ui->strl_btn->setEnabled(true);
//}

/*设置相机的分辨率，一定要在bin mode函数调用之后立即调用*/
void Guider::SetResolutin()
{
    ret = GetQHYCCDChipInfo(camhandle,&chipw,&chiph,&imagew,&imageh,&pixelw,&pixelh,&bpp);
    if(ret == QHYCCD_SUCCESS){
        ret = SetQHYCCDResolution(camhandle,0,0,imagew/wbin,imageh/hbin);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDResolution(camhandle,0,0,iamgew/wbin,imageh/hbin);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's resolution,and you need use this function after 'SetQHYCCDBinMode()',"
                                 "and if you use this function set ROI,you need notice parameter2 + parameter4 < imagew,parameter3 + parameter5 < imageh."
                                 "\n"
                                 "Function run result:\n"
                                 "Set resolution successfully.");
            //flag_rsln_clicked = true;
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Using method:\n"
                                 "int ret = QHYCCD_SUCCESS;\n"
                                 "ret = SetQHYCCDResolution(camhandle,0,0,iamgew/wbin,imageh/hbin);\n"
                                 "\n"
                                 "Introduction:\n"
                                 "    This function can set camera's resolution,and you need use this function after 'SetQHYCCDBinMode()',"
                                 "and if you use this function set ROI,you need notice parameter2 + parameter4 < imagew,parameter3 + parameter5 < imageh."
                                 "\n"
                                 "Function run result:\n"
                                 "Set resolution failed.");
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Get QHYCCD chip information failed.");
    }
}

/*开始单帧曝光*/
void Guider::ExpSingleFrame()
{
    canGetTemp = false;
    ret = ExpQHYCCDSingleFrame(camhandle);
    if(ret != QHYCCD_ERROR){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = ExpQHYCCDSingleFrame(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera's start single expose."
                             "\n"
                             "Function run result:\n"
                             "Exposing single frame successfully.");

//        length = GetQHYCCDMemLength(camhandle);
//        if(length > 0)
//        {
//            printf("length = %d\n",length);
//            cam.length = length;
//            cam.ImgData = (unsigned char *)malloc(cam.length);

//        }

        flag_exped = true;//用来判断相机是否已经曝光的标志位，后面的获取数据，停止曝光等功能需要用到
        ui->epsg_btn->setEnabled(false);
        ui->gtsg_btn->setEnabled(true);
        ui->cerd_btn->setEnabled(true);
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = ExpQHYCCDSingleFrame(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera start single expose."
                             "\n"
                             "Function run result:\n"
                             "Exposing single frame failed.");
    }

}

QImage* Guider::IplImage2QImage(IplImage *iplImg)
{
    int h = iplImg->height;
    int w = iplImg->width;
    int channels = iplImg->nChannels;

    QImage *qimg = new QImage(w, h, QImage::Format_RGB888);
    char *data = iplImg->imageData;

    for (int y = 0; y < h; y++, data += iplImg->widthStep)
    {
        for (int x = 0; x < w; x++)
        {
            if(channels == 3)
            {
                char r, g, b;//, a = 0;
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
                qimg->setPixel(x, y, qRgb(r, g, b));
            }
            else
            {
                char r, g, b;//, a = 0;
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
                qimg->setPixel(x, y, qRgb(r, g, b));
            }
        }
    }
    return qimg;
}

void Guider::ShowImage()
{
    IplImage *image = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
    if(cam.bpp == 16){
        for (int i = 0; i < cam.imagew * cam.imageh * cam.channels; i++){
            image->imageData[i] = (cam.ImgData[2*i + 1] * 256 + cam.ImgData[2*i]) >> 8;
        }
    }else{
        for (int i = 0; i < cam.imagew * cam.imageh * cam.channels; i++){
            image->imageData[i] = cam.ImgData[i];//cam.ImgData[i];
        }
    }

//    cvNamedWindow("1",0);
//    cvShowImage("1",image);

    //resize image size and show
    CvSize dst_cvsize;
    dst_cvsize.width  = ui->img_lbl->width();
    dst_cvsize.height = ui->img_lbl->height();
    IplImage *dst = cvCreateImage(dst_cvsize,image->depth,image->nChannels);
    cvResize(image,dst,CV_INTER_AREA);//CV_INTER_LINEAR);

//    cvNamedWindow("2",0);
//    cvShowImage("2",dst);

    int w = dst->width;
    int h = dst->height;
    int channels = dst->nChannels;

    QImage *qImg_capture = new QImage(w, h, QImage::Format_RGB888);
    char *data = dst->imageData;

    for (int y = 0; y < h; y++, data += dst->widthStep)
    {
        for (int x = 0; x < w; x++)
        {
            if(channels == 3)
            {
                char r, g, b;//, a = 0;
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
                qImg_capture->setPixel(x, y, qRgb(r, g, b));
            }
            else
            {
                char r, g, b;//, a = 0;
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
                qImg_capture->setPixel(x, y, qRgb(r, g, b));
            }
        }
    }

    ui->img_lbl->setScaledContents(true);
    ui->img_lbl->setPixmap(QPixmap::fromImage(*qImg_capture));
    ui->img_lbl->show();

    cvReleaseImage(&image);
    cvReleaseImage(&dst);
    delete qImg_capture;

    cam.canget = 1;

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    ui->opt_tbs->clear();
    ui->opt_tbs->setText(current_date);
}

/*获取图像数据，并显示在QLabel控件上*/
void Guider::GetSingleFrame()
{
    cam.canTransferData = true;
    cam.getData = 0;
    smt = new SingleModeThread(this);
    connect(smt, SIGNAL(finished()), smt, SLOT(deleteLater()));
    smt->start();
    while(cam.getData == 0)
    {
        QApplication::processEvents();//响应界面操作，防止界面假死
    }

    ShowImage();
    cam.getData = 0;

    ui->gtsg_btn->setEnabled(false);
    ui->singleSaveBtn->setEnabled(true);
    ui->singleFormatCbx->setEnabled(true);

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    ui->opt_tbs->clear();
    ui->opt_tbs->setText(current_date);
//    ui->opt_tbs->clear();
//    ui->opt_tbs->setText("Using method:\n"
//                         "int ret = QHYCCD_SUCCESS;\n"
//                         "ret = GetQHYCCDSingleFrame(camhandle,&imagew,&imageh,&bpp,&channels,ImgData);\n"
//                         "\n"
//                         "Introduction:\n"
//                         "    This function can get camera's image data.And about this function's parameters,you just need define them,"
//                         "don't set value for them."
//                         "\n"
//                         "Function run result:\n"
//                         "Get single frame successfully.\n");
}

/*下面两个函数是关闭相机曝光时用到的函数，不同之处在于CancelQHYCCDExposingAndReadout执行后不需要读出数据，
 *但是CancelQHYCCDExposing执行后仍然需要读出数据，并且这个差异只针对老型号的相机，对新型号的相机，两者的作
 *用和使用方法是相同的。*/
/*关闭相机曝光和数据读出*/
void Guider::CancelExposingAndReadout()
{
    ret = CancelQHYCCDExposingAndReadout(camhandle);
    if(ret == QHYCCD_SUCCESS){
        canGetTemp = true;

//        if(cam.ImgDataSave != NULL)
//            free(cam.ImgDataSave);
//        if(cam.ImgData != NULL)
//            free(cam.ImgData);

        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = CancelQHYCCDExposingAndReadout(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera's end expose.And for old model camera,'CancelQHYCCDExposingAndReadout' and "
                             "'CancelQHYCCDExposing' have some differences,'CancelQHYCCDExposingAndReadout' just end expose,but if you run "
                             "'CancelQHYCCDExposing',you still need run 'GetQHYCCDSingleFrame()' to get image."
                             "\n"
                             "Function run result:\n"
                             "Cancel exposing and readout successfully.");
        //free(ImgData);
        ui->epsg_btn->setEnabled(true);
        ui->gtsg_btn->setEnabled(false);
        ui->cerd_btn->setEnabled(false);
//        ui->singleSaveBtn->setEnabled(false);
//        ui->cexp_btn->setEnabled(false);
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = CancelQHYCCDExposingAndReadout(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera's end expose.And for old model camera,'CancelQHYCCDExposingAndReadout' and "
                             "'CancelQHYCCDExposing' have some differences,'CancelQHYCCDExposingAndReadout' just end expose,but if you run "
                             "'CancelQHYCCDExposing',you still need run 'GetQHYCCDSingleFrame()' to get image."
                             "\n"
                             "Function run result:\n"
                             "Cancel exposing and readout failed");
    }
}

/*关闭相机曝光，但仍需数据读出*/
void Guider::CancelExposing()
{
    ret = CancelQHYCCDExposing(camhandle);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = CancelQHYCCDExposing(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera's end expose.And for old model camera,'CancelQHYCCDExposingAndReadout' and "
                             "'CancelQHYCCDExposing' have some differences,'CancelQHYCCDExposingAndReadout' just end expose,but if you run "
                             "'CancelQHYCCDExposing',you still need run 'GetQHYCCDSingleFrame()' to get image."
                             "\n"
                             "Function run result:\n"
                             "Cancel exposing successfully.");
        ui->epsg_btn->setEnabled(true);
        ui->gtsg_btn->setEnabled(false);
        ui->cerd_btn->setEnabled(false);
//        ui->cexp_btn->setEnabled(false);
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = CancelQHYCCDExposing(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera's end expose.And for old model camera,'CancelQHYCCDExposingAndReadout' and "
                             "'CancelQHYCCDExposing' have some differences,'CancelQHYCCDExposingAndReadout' just end expose,but if you run "
                             "'CancelQHYCCDExposing',you still need run 'GetQHYCCDSingleFrame()' to get image."
                             "\n"
                             "Function run result:\n"
                             "Cancel exposing failed");
    }
}

/*开始相机连续模式曝光*/
void Guider::BeginLive()
{
    ret = BeginQHYCCDLive(camhandle);
    if(ret == QHYCCD_SUCCESS){
//        printf("length = %d\n",cam.length);
//        cam.ImgData = (unsigned char *)malloc(cam.length);
//        memset(cam.ImgData,0,cam.length*sizeof(char));
//        usleep(100);
//        cam.ImgDataSave = (unsigned char *)malloc(cam.length);
//        memset(cam.ImgDataSave,0,cam.length*sizeof(char));

        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = BeginQHYCCDLive(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera start expose."
                             "\n"
                             "Function run result:\n"
                             "Begin live successfully.");

        cam.quit = 0;

        ui->bglv_btn->setEnabled(false);
        ui->gtlv_btn->setEnabled(true);
        ui->stpl_btn->setEnabled(true);
        flag_lived = true;
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = BeginQHYCCDLive(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera start expose."
                             "\n"
                             "Function run result:\n"
                             "Begin live failed.");
    }
}

/*点击GetLive按键时调用的函数，会在此函数中新建一个线程，并使用定时器计算持续时间和帧率*/
void Guider::GetLiveFrame()
{
    cam.quit = 0;
    cam.canget = 1;
    cam.canTransferData = true;
    lmt = new LiveModeThread(this);
    connect(lmt,SIGNAL(gotLiveData()),this,SLOT(ShowImage()));
    connect(lmt,SIGNAL(finished()),lmt,SLOT(deleteLater()));
    lmt->start();
    ui->gtlv_btn->setEnabled(false);
    ui->liveSaveBtn->setEnabled(true);
    ui->liveFormatCbx->setEnabled(true);
    ui->opt_tbs->clear();
    ui->opt_tbs->setText("Using method:\n"
                         "int ret = QHYCCD_SUCCESS;\n"
                         "ret = GetQHYCCDLiveFrame(camhandle,&imagew,&imageh,&bpp,&channels,ImgData);\n"
                         "\n"
                         "Introduction:\n"
                         "    This function can get camera's image data.And about this function's parameters,you just need define them,"
                         "don't set value for them."
                         "\n"
                         "Function run result:\n"
                         "Get live frame successfully.\n");
}

/*停止连续模式的函数，结束线程，同时关闭定时器*/
void Guider::StopLiveFrame()
{
    cam.quit = 1;
    cam.hasquit = 0;

    while(cam.hasquit != 1)
    {
        QApplication::processEvents();
    }

//    if(cam.ImgData != NULL)
//        free(cam.ImgData);
//    if(cam.ImgDataSave != NULL)
//        free(cam.ImgDataSave);

    ret = StopQHYCCDLive(camhandle);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = StopQHYCCDLive(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera stop expose."
                             "\n"
                             "Function run result:\n"
                             "Stop live successfully.");
//        cam.quit = 1;
        ui->bglv_btn->setEnabled(true);
        ui->gtlv_btn->setEnabled(false);
        ui->stpl_btn->setEnabled(false);
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Using method:\n"
                             "int ret = QHYCCD_SUCCESS;\n"
                             "ret = StopQHYCCDLive(camhandle);\n"
                             "\n"
                             "Introduction:\n"
                             "    This function can set camera stop expose."
                             "\n"
                             "Function run result:\n"
                             "Stop live failed.");
    }
}

void Guider::ControlAvailable()
{
    int i = ui->isal_cbbx->currentIndex();
    uint32_t ret = QHYCCD_ERROR;

    if(i == 0){
        ui->opt_tbs->clear();
    }else{
        ret = IsQHYCCDControlAvailable(camhandle,(enum CONTROL_ID)(i-1));
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->setText("Introduction:\n"
                                 "    This camera support this function.You can check if camera support this function "
                                 "by 'ret = IsQHYCCDControlAvailable(camhandle," + idList[i-1] + ")'.");
        }else{
            ui->opt_tbs->setText("Introduction:\n"
                                 "    This camera doesn't support this function.You can check if camera support this function "
                                 "by 'ret = IsQHYCCDControlAvailable(camhandle," + idList[i-1] + ")'.");
        }
    }
}

void Guider::IsCFWPlugged()
{
    uint32_t ret = QHYCCD_ERROR;
    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_CFWPORT);
    if(ret == QHYCCD_SUCCESS){
        ret = IsQHYCCDCFWPlugged(camhandle);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    CFW has been plugged.You can use 'ret = IsQHYCCDCFWPlugged(camhandle);' check it.");
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    CFW hasn't been plugged.You can use 'ret = IsQHYCCDCFWPlugged(camhandle);' check it.");
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support color filter wheel function,and you can check if camera support "
                             "color filter wheel by 'ret = IsQHYCCDControlAvailable(camhandle,QHYCCD_CFWPORT);'");
    }
}

void Guider::GetCFWStatus()
{
    uint32_t ret = QHYCCD_ERROR;
    char status;
    ret = GetQHYCCDCFWStatus(camhandle,&status);
    if(ret == QHYCCD_SUCCESS){
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    Current CFW status is " + QString::number(status,16,0) + "this is a ASCII.You can use 'ret "
                             "= GetQHYCCDCFWStatus(camhandle,&status);' get it,but some camera don't support this function.");
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    Get CFW status fail,and some camera don't support this function.");
    }
}

void Guider::GetCFWSlotsNum()
{
    uint32_t ret = QHYCCD_SUCCESS;
    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_CFWSLOTSNUM);
    if(ret == QHYCCD_SUCCESS){
        ret = GetQHYCCDParam(camhandle,CONTROL_CFWSLOTSNUM);
        if(ret != QHYCCD_ERROR){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    This CFW number is " + QString::number(ret,16,0) + ",you can use 'ret = GetQHYCCDParam(camhandle,"
                                 "CONTROL_CFWSLOTSNUM);' get it,and some camera don't support this function.");
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    Get CFW number ail,you can use 'ret = GetQHYCCDParam(camhandle,CONTROL_CFWSLOTSNUM);' "
                                 "get it,and some camera don't support this function.");
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support this functio,you can check if camera support this function by 'ret = "
                             "IsQHYCCDControlAvailable(camhandle,QHYCCD_CFWPORT);'.");
    }
}

void Guider::SendOrder2CFW()
{
    char order = 47 + ui->odtc_cbbx->currentIndex();
    uint32_t ret = QHYCCD_ERROR;

    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_CFWPORT);
    if(ret == QHYCCD_SUCCESS){
        if(ui->odtc_cbbx->currentIndex() == 0){
            ui->opt_tbs->clear();
        }else{
            ret = SendOrder2QHYCCDCFW(camhandle,&order,1);
            if(ret == QHYCCD_SUCCESS){
                ui->opt_tbs->clear();
                ui->opt_tbs->setText("Introduction:\n"
                                     "    Send order to CFW success.You can use 'ret = SendOrder2QHYCCDCFW(camhandle,order,1);' set it.");
            }else{
                ui->opt_tbs->clear();
                ui->opt_tbs->setText("Introduction:\n"
                                     "    Send order to CFW fail.You can use 'ret = SendOrder2QHYCCDCFW(camhandle,order,1);' set it.");
            }
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support color filter wheel function,and you can check if camera support "
                             "color filter wheel by 'ret = IsQHYCCDControlAvailable(camhandle,QHYCCD_CFWPORT);'");
    }
}

void Guider::on_sttp_btn_clicked()
{
    double temp;
    uint32_t ret = QHYCCD_ERROR;

    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_COOLER);
    if(ret == QHYCCD_SUCCESS){
        temp = ui->temp_spbx->value();
        ret = ControlQHYCCDTemp(camhandle,temp);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    Set temperature to camera success.You can use 'ret = ControlQHYCCDTemp(camhandle,temp);' set it,"
                                 "and you also can use 'SetQHYCCDParam' set it.");
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    Set temperature to camera fail.You can use 'ret = ControlQHYCCDTemp(camhandle,temp);' set it,"
                                 "and you also can use 'SetQHYCCDParam' set it.");
        }
    }
    else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support cooler function,and you can check if camera support cooler function by 'ret = "
                             "IsQHYCCDControlAvailable(camhandle,QHYCCD_COOLER);'");
    }
}

//bool gtpquit = true;

void Guider::on_gttp_btn_clicked()
{
    double temp;
    uint32_t ret = QHYCCD_ERROR;

//    if(gtpquit == true)
//        gtpquit == false;
//    else
//        gtpquit == true;

//    while(1){
//        temp = GetQHYCCDParam(camhandle,CONTROL_CURTEMP);
//        ui->opt_tbs->clear();
//        ui->opt_tbs->setText("Introduction:\n"
//                             "    Current temperature is " + QString::number(temp,16,1) + "°C.You can use 'temp = GetQHYCCDParam("
//                             "camhandle,CONTROL_CURTEMP);' get it");
//        sleep(1);
//        if(gtpquit == true)
//            break;
//    }

    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_COOLER);
    if(ret == QHYCCD_SUCCESS){
        temp = GetQHYCCDParam(camhandle,CONTROL_CURTEMP);
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    Current temperature is " + QString::number(temp,16,1) + "°C.You can use 'temp = GetQHYCCDParam("
                             "camhandle,CONTROL_CURTEMP);' get it");
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support cooler function,and you can check if camera support cooler function by 'ret = "
                             "IsQHYCCDControlAvailable(camhandle,QHYCCD_COOLER);'");
    }

    timer = this->startTimer(1000);
//    timer = this->sto
//    update_timer->start(1000);
}

void Guider::on_spwm_btn_clicked()
{
    int pwm;
    uint32_t ret = QHYCCD_ERROR;

    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_COOLER);
    if(ret == QHYCCD_SUCCESS){
        pwm = ui->pwm_sbpx->value();
        ret = SetQHYCCDParam(camhandle,CONTROL_MANULPWM,(double)pwm);
        if(ret == QHYCCD_SUCCESS){
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    Set PWM to camera success.You can use 'ret = SetQHYCCDParam(camhandle,CONTROL_MANULPWM,pwm);' set it.");
        }else{
            ui->opt_tbs->clear();
            ui->opt_tbs->setText("Introduction:\n"
                                 "    Set PWM to camera fail.You can use 'ret = SetQHYCCDParam(camhandle,CONTROL_MANULPWM,pwm);' set it.");
        }
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support cooler function,and you can check if camera support cooler function by 'ret = "
                             "IsQHYCCDControlAvailable(camhandle,QHYCCD_COOLER);'");
    }
}

void Guider::on_gpwm_btn_clicked()
{
    double pwm;
    uint32_t ret = QHYCCD_ERROR;

    ret = IsQHYCCDControlAvailable(camhandle,CONTROL_COOLER);
    if(ret == QHYCCD_SUCCESS){
        pwm = GetQHYCCDParam(camhandle,CONTROL_CURPWM);
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    Current PWM is " + QString::number((pwm / 255.0) * 100,16,1) + "%,setting value is " + QString::number(pwm,16,0) +
                             ".You can use 'temp = GetQHYCCDParam(camhandle,CONTROL_CURPWM);' get it");
    }else{
        ui->opt_tbs->clear();
        ui->opt_tbs->setText("Introduction:\n"
                             "    This camera don't support cooler function,and you can check if camera support cooler function by 'ret = "
                             "IsQHYCCDControlAvailable(camhandle,QHYCCD_COOLER);'");
    }
}

void Guider::on_pushButton_2_clicked()
{
    if(b1 == 1)
    {
        ui->pushButton_2->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox->setVisible(false);
        b1 = b1 * (-1);
    }
    else
    {
        ui->pushButton_2->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox->setVisible(true);
        b1 = b1 * (-1);
    }
}

void Guider::on_pushButton_3_clicked()
{
    if(b2 == 1)
    {
        ui->pushButton_3->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_2->setVisible(false);
        b2 = b2 * (-1);
    }
    else
    {
        ui->pushButton_3->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_2->setVisible(true);
        b2 = b2 * (-1);
    }
}

void Guider::on_pushButton_4_clicked()
{
    if(b3 == 1)
    {
        ui->pushButton_4->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_3->setVisible(false);
        b3 = b3 * (-1);
    }
    else
    {
        ui->pushButton_4->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_3->setVisible(true);
        b3 = b3 * (-1);
    }
}

void Guider::on_pushButton_5_clicked()
{
    if(b4 == 1)
    {
        ui->pushButton_5->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_4->setVisible(false);
        b4 = b4 * (-1);
    }
    else
    {
        ui->pushButton_5->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_4->setVisible(true);
        b4 = b4 * (-1);
    }
}

void Guider::on_pushButton_6_clicked()
{
    if(b5 == 1)
    {
        ui->pushButton_6->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_5->setVisible(false);
        b5 = b5 * (-1);
    }
    else
    {
        ui->pushButton_6->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_5->setVisible(true);
        b5 = b5 * (-1);
    }
}

void Guider::on_pushButton_7_clicked()
{
    if(b6 == 1)
    {
        ui->pushButton_7->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_6->setVisible(false);
        b6 = b6 * (-1);
    }
    else
    {
        ui->pushButton_7->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_6->setVisible(true);
        b6 = b6 * (-1);
    }
}

void Guider::on_pushButton_8_clicked()
{
    if(b7 == 1)
    {
        ui->pushButton_8->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_7->setVisible(false);
        b7 = b7 * (-1);
    }
    else
    {
        ui->pushButton_8->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_7->setVisible(true);
        b7 = b7 * (-1);
    }
}

void Guider::on_pushButton_9_clicked()
{
    if(b8 == 1)
    {
        ui->pushButton_9->setStyleSheet("background-image: url(:/image/image/unfoldHead.bmp);");
        ui->groupBox_8->setVisible(false);
        b8 = b8 * (-1);
    }
    else
    {
        ui->pushButton_9->setStyleSheet("background-image: url(:/image/image/foldHead.bmp);");
        ui->groupBox_8->setVisible(true);
        b8 = b8 * (-1);
    }
}


void Guider::on_exp_sld_valueChanged(int value)
{
    //printf("exp_slider_valueChanged %d\n",value);
    ui->exp_pbx->setValue(value);
    if(ui->unt_btn->text() == "ms"){
        ret = SetQHYCCDParam(camhandle,CONTROL_EXPOSURE,(double)(value * 1000));
        cam.exptime = ui->exp_sld->value() * 1000;
    }else{
        ret = SetQHYCCDParam(camhandle,CONTROL_EXPOSURE,(double)(value * 1000 * 1000));
        cam.exptime = ui->exp_sld->value() * 1000 * 1000;
    }
}

void Guider::on_unt_btn_clicked()
{
    if(ui->unt_btn->text() == "ms"){
        ui->unt_btn->setText("s");
        ret = SetQHYCCDParam(camhandle,CONTROL_EXPOSURE,(double)(ui->exp_sld->value() * 1000 * 1000));
        cam.exptime = ui->exp_sld->value() * 1000 * 1000;
    }
    else{
        ui->unt_btn->setText("ms");
        ret = SetQHYCCDParam(camhandle,CONTROL_EXPOSURE,(double)(ui->exp_sld->value() * 1000));
        cam.exptime = ui->exp_sld->value() * 1000;
    }
}

void Guider::on_exp_pbx_valueChanged(double arg1)
{
//    printf("%d\n",arg1);
    ui->exp_sld->setValue(arg1);
}

void Guider::on_gan_sld_valueChanged(int value)
{
    ui->gan_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_GAIN,(double)value);
    cam.gain = value;
}

void Guider::on_gan_pbx_valueChanged(double arg1)
{
    ui->gan_sld->setValue(arg1);
}

void Guider::on_oft_sld_valueChanged(int value)
{
    ui->oft_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_OFFSET,(double)value);
    cam.offset = value;
}

void Guider::on_oft_pbx_valueChanged(double arg1)
{
    ui->oft_sld->setValue(arg1);
}

void Guider::on_spd_sld_valueChanged(int value)
{
    ui->spd_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_SPEED,(double)value);
}

void Guider::on_spd_pbx_valueChanged(double arg1)
{
    ui->spd_sld->setValue(arg1);
}

void Guider::on_tfc_sld_valueChanged(int value)
{
    ui->tfc_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_USBTRAFFIC,(double)value);
}

void Guider::on_tfc_pbx_valueChanged(double arg1)
{
    ui->tfc_sld->setValue(arg1);
}

void Guider::on_wbr_sld_valueChanged(int value)
{
    ui->wbr_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_WBR,(double)value);
}

void Guider::on_wbr_pbx_valueChanged(double arg1)
{
    ui->wbr_sld->setValue(arg1);
}

void Guider::on_wbg_sld_valueChanged(int value)
{
    ui->wbg_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_WBG,(double)value);
}

void Guider::on_wbg_pbx_valueChanged(double arg1)
{
    ui->wbg_sld->setValue(arg1);
}

void Guider::on_wbb_sld_valueChanged(int value)
{
    ui->wbb_pbx->setValue(value);
    ret = SetQHYCCDParam(camhandle,CONTROL_WBB,(double)value);
}

void Guider::on_wbb_pbx_valueChanged(double arg1)
{
    ui->wbb_sld->setValue(arg1);
}

void Guider::on_brt_sld_valueChanged(int value)
{
    printf("value = %d %f\n",value,(double)value / 10);
    ui->brt_pbx->setValue((double)value / 10);

}

void Guider::on_brt_pbx_valueChanged(double arg1)
{
    printf("arg1 = %f\n",arg1);
    ui->brt_sld->setValue(doubleToInt(arg1 * 10));
    ret = SetQHYCCDParam(camhandle,CONTROL_BRIGHTNESS,arg1);
}

void Guider::on_ctt_sld_valueChanged(int value)
{
    ui->ctt_pbx->setValue((double)value / 10);
//    ret = SetQHYCCDParam(camhandle,CONTROL_CONTRAST,(double)value);
}

void Guider::on_ctt_pbx_valueChanged(double arg1)
{
    ui->ctt_sld->setValue(doubleToInt(arg1 * 10));
    ret = SetQHYCCDParam(camhandle,CONTROL_CONTRAST,arg1);
}

void Guider::on_gma_sld_valueChanged(int value)
{
    ui->gma_pbx->setValue((double)value / 10);

}

void Guider::on_gma_pbx_valueChanged(double arg1)
{
    ui->gma_sld->setValue(doubleToInt(arg1 * 10));
    ret = SetQHYCCDParam(camhandle,CONTROL_GAMMA,arg1);
}

void Guider::on_clr_cbx_currentTextChanged(const QString &arg1)
{
    if(arg1 == "ON")
        ret = SetQHYCCDParam(camhandle,CAM_COLOR,1);
    else
        ret = SetQHYCCDParam(camhandle,CAM_COLOR,0);
}

void Guider::on_bit_cbx_currentTextChanged(const QString &arg1)
{
//    printf("***************************************\n");
    if(arg1 == "8Bits"){
        ret = SetQHYCCDParam(camhandle,CONTROL_TRANSFERBIT,8);
    }else{
        ret = SetQHYCCDParam(camhandle,CONTROL_TRANSFERBIT,16);
        ret = SetQHYCCDDebayerOnOff(camhandle,false);
        ui->clr_cbx->setCurrentIndex(1);
    }
}

void Guider::on_bin_cbx_currentTextChanged(const QString &arg1)
{
    printf("Set BIN");
    if(arg1 == "1X1"){
        ret = SetQHYCCDBinMode(camhandle,1,1);
        ret = SetQHYCCDResolution(camhandle,0,0,imagew / 1,imageh / 1);
        cam.bin = 1;
    }else if( arg1 == "2X2"){
        ret = SetQHYCCDBinMode(camhandle,2,2);
        ret = SetQHYCCDResolution(camhandle,0,0,imagew / 2,imageh / 2);
        cam.bin = 2;
    }else if(arg1 == "3X3"){
        ret = SetQHYCCDBinMode(camhandle,3,3);
        ret = SetQHYCCDResolution(camhandle,0,0,imagew / 3,imageh / 3);
        cam.bin = 3;
    }else{
        ret = SetQHYCCDBinMode(camhandle,4,4);
        ret = SetQHYCCDResolution(camhandle,0,0,imagew / 4,imageh / 4);
        cam.bin = 4;
    }
}

void Guider::on_roi_btn_clicked()
{
    printf("%d %d %d %d %d %d\n",ui->stx_pbx->value(),ui->szx_pbx->value(),cam.maximgw,ui->sty_pbx->value(),ui->szy_pbx->value(),cam.maximgh);
    if((ui->stx_pbx->value() + ui->szx_pbx->value() <= cam.maximgw) && (ui->sty_pbx->value() + ui->szy_pbx->value() <= cam.maximgh))
        ret = SetQHYCCDResolution(camhandle,ui->stx_pbx->value(),ui->sty_pbx->value(),ui->szx_pbx->value(),ui->szy_pbx->value());
    else
        printf("set wrong\n");
}

void Guider::on_ost_cbx_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Other Setting"){
        ui->ost_pbx->setValue(0);
        ui->ost_sld->setValue(0);

        ui->ost_pbx->setEnabled(false);
        ui->ost_sld->setEnabled(false);
    }else{
        ui->ost_pbx->setEnabled(true);
        ui->ost_sld->setEnabled(true);

        int i,index;
        for(i = 0;i < 53;i ++){
            if(arg1 == idList[i])
                index = i;
        }

        double min,max,step;
        ret = IsQHYCCDControlAvailable(camhandle,(CONTROL_ID)index);
        if(ret == QHYCCD_SUCCESS){
            ret = GetQHYCCDParamMinMaxStep(camhandle,(CONTROL_ID)index,&min,&max,&step);
            if(ret == QHYCCD_SUCCESS){
                ui->ost_sld->setMinimum(min);
                ui->ost_sld->setMaximum(max);
                ui->ost_sld->setPageStep(step);

                ui->ost_pbx->setMinimum(min);
                ui->ost_pbx->setMaximum(max);
                ui->ost_pbx->setSingleStep(step);
            }else{
                ui->ost_sld->setMinimum(0);
                ui->ost_sld->setMaximum(1);
                ui->ost_sld->setPageStep(1);

                ui->ost_pbx->setMinimum(0);
                ui->ost_pbx->setMaximum(1);
                ui->ost_pbx->setSingleStep(1);
            }
        }
    }
}

void Guider::on_ost_sld_valueChanged(int value)
{
    ui->ost_pbx->setValue(value);
}

void Guider::on_ost_pbx_valueChanged(int arg1)
{
    ui->ost_pbx->setValue(arg1);
}

void Guider::writeFITHeader(fitsfile *fptr, QString iDataType, QString iKey, QString iValue, QString iDesc, int status)
{
    char ss[81];
    char keywords[40];
    char description[40];

    QByteArray ba_key;
    QByteArray ba_value;
    QByteArray ba_desc;

    ba_key = iKey.toLatin1();
    strcpy(keywords,ba_key.data());
    ba_value = iValue.toLatin1();
    strcpy(ss,ba_value.data());
    ba_desc = iDesc.toLatin1();
    strcpy(description,ba_desc.data());

    if(iDataType.compare("TSTRING") == 0)
    {
        fits_update_key(fptr, TSTRING, keywords, &ss, description, &status);
    }
    else if(iDataType.compare("TINT") == 0)
    {
        bool ok;
        int v = iValue.toInt(&ok, 10);
        if(ok)
            fits_update_key(fptr, TINT, keywords, &v, description, &status);
    }
    else if(iDataType.compare("TDOUBLE") == 0)
    {
        bool ok;
        double v = iValue.toDouble(&ok);
        if(ok)
            fits_update_key(fptr, TDOUBLE, keywords, &v, description, &status);
    }
    else if(iDataType.compare("TLOGICAL") == 0)
    {
        fits_update_key(fptr, TLOGICAL, keywords, &ss, description, &status);
    }
}

void Guider::saveAsFIT(QString filename)
{
    //创建一个IplImage变量，用来存储要保存的数据，使用OpenCV库
    IplImage *FitImg;
    FitImg = cvCreateImage(cvSize(cam.imagew,cam.imageh), cam.bpp, cam.channels);
    FitImg->imageData = (char*)cam.ImgDataSave;

//    cvNamedWindow("11",0);
//    cvShowImage("11",FitImg);
//    cvWaitKey(0);

    //下面几个变量不用管，是保存FITS文件是需要的变量
    fitsfile *fptr;
    int status;
    long  fpixel = 1, naxis = 2, nelements;
    long naxes[2] = { cam.imagew, cam.imageh };   //数组的两个元素分别是图像宽度和高度

    status = 0;         //开始保存前初始化状态

    std::string str = filename.toStdString();
    const char* ch = str.c_str();
    fits_create_file(&fptr, ch, &status);//创建FITS文件

    //创建文件的图像数据空间
    if(cam.bpp == 16)
        fits_create_img(fptr, USHORT_IMG, naxis, naxes, &status);
    else
        fits_create_img(fptr, BYTE_IMG, naxis, naxes, &status);

//    //写FITS文件头的信息
    writeFITHeader(fptr,"TSTRING","PRODUCT","QHYCCD","manufacturer of camera",status);
    writeFITHeader(fptr,"TINT","GAIN",QString::number(cam.gain),"gain of image",status);
    writeFITHeader(fptr,"TINT","OFFSET",QString::number(cam.offset),"offset of image",status);
    writeFITHeader(fptr,"TINT","EXPTIME",QString::number(cam.exptime),"expose time of image",status);
    writeFITHeader(fptr,"TINT","BIN",QString::number(cam.bin),"bin of image",status);
    writeFITHeader(fptr,"TINT","WIDTH",QString::number(cam.imagew),"width of image",status);
    writeFITHeader(fptr,"TINT","HEIGHT",QString::number(cam.imageh),"height of image",status);
    writeFITHeader(fptr,"TINT","BITS",QString::number(cam.bpp),"bits of image",status);
    writeFITHeader(fptr,"TINT","CHANNELS",QString::number(cam.channels),"channels of image",status);
    printf("%f %f %f %f\n",cam.chipw,cam.chiph,cam.pixelw,cam.pixelh);
    writeFITHeader(fptr,"TDOUBLE","CHIPW",QString::number(cam.chipw, 10, 6),"chip width of camera",status);
    writeFITHeader(fptr,"TDOUBLE","CHIPH",QString::number(cam.chiph, 10, 6),"chip height of camera",status);
    writeFITHeader(fptr,"TDOUBLE","PIXELW",QString::number(cam.pixelw, 10, 6),"pixel width of camera",status);
    writeFITHeader(fptr,"TDOUBLE","PIXELH",QString::number(cam.pixelh, 10, 6),"pixel height of camera",status);

    nelements = naxes[0] * naxes[1];          //写入的像素数量

    //开始写入图像数据，写入时，不使用IplImage直接用GetQHYCCDSingleFrame返回的ImgData也可以
    if(cam.bpp == 16)
        fits_write_img(fptr, TUSHORT, fpixel, nelements, &FitImg->imageData[0], &status);//
    else
        fits_write_img(fptr, TBYTE, fpixel, nelements, &FitImg->imageData[0], &status);

    fits_close_file(fptr, &status);            //写入完成后关闭文件
    fits_report_error(stderr, status);  //若保存失败，打印错误信息

    FitImg->imageData=0;
    cvReleaseImage(&FitImg);
}

void Guider::on_singleSaveBtn_clicked()
{
    ui->singleFormatCbx->setEnabled(false);
    cam.canTransferData = false;

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmss");

    //保存的路径和文件名，注意保存目录下不能有同名文件，否则无法保存
    QString filename = QCoreApplication::applicationDirPath() + "/" + QString::fromUtf8(cam.model) + "-" + current_date;// + ".fit";
    printf("filename = %s\n",qPrintable(filename));

    if(ui->singleFormatCbx->currentText() == "FIT"){
        filename = filename + ".fit";
        this->saveAsFIT(filename);
    }else if(ui->singleFormatCbx->currentText() == "TIFF"){
        IplImage *ImgTIFTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".tif";
        if(cam.bpp == 8){
//            ImgTIFTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgTIFTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgTIFTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgTIFTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgTIFTemp,0);
            cvReleaseImage(&ImgTIFTemp);
        }
    }else if(ui->singleFormatCbx->currentText() == "JPG"){
        IplImage *ImgJPGTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".jpg";
        if(cam.bpp == 8){
//            ImgJPGTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgJPGTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgJPGTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgJPGTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgJPGTemp,0);
            cvReleaseImage(&ImgJPGTemp);
        }
    }else if(ui->singleFormatCbx->currentText() == "PNG"){
        IplImage *ImgPNGTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".png";
        if(cam.bpp == 8){
//            ImgPNGTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgPNGTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgPNGTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgPNGTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgPNGTemp,0);
            cvReleaseImage(&ImgPNGTemp);
        }
    }else{
        IplImage *ImgBMPTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".bmp";
        if(cam.bpp == 8){
//            ImgBMPTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgBMPTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgBMPTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgBMPTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgBMPTemp,0);
            cvReleaseImage(&ImgBMPTemp);
        }
    }

    cam.canTransferData = true;

    ui->singleFormatCbx->setEnabled(true);
    ui->opt_tbs->setText("Save" + ui->singleFormatCbx->currentText() + " format image successfully.");
}

void Guider::on_liveSaveBtn_clicked()
{
    ui->liveFormatCbx->setEnabled(false);
    cam.canTransferData = false;

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmss");

    //保存的路径和文件名，注意保存目录下不能有同名文件，否则无法保存
    QString filename = QCoreApplication::applicationDirPath() + "/" + QString::fromUtf8(cam.model) + "-" + current_date;// + ".fit";
    printf("filename = %s\n",qPrintable(filename));

    if(ui->liveFormatCbx->currentText() == "FIT"){
        filename = filename + ".fit";
        this->saveAsFIT(filename);
    }else if(ui->liveFormatCbx->currentText() == "TIFF"){
        IplImage *ImgTIFTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".tif";
        if(cam.bpp == 8){
//            ImgTIFTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgTIFTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgTIFTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgTIFTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgTIFTemp,0);
            cvReleaseImage(&ImgTIFTemp);
        }
    }else if(ui->liveFormatCbx->currentText() == "JPG"){
        IplImage *ImgJPGTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".jpg";
        if(cam.bpp == 8){
//            ImgJPGTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgJPGTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgJPGTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgJPGTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgJPGTemp,0);
            cvReleaseImage(&ImgJPGTemp);
        }
    }else if(ui->liveFormatCbx->currentText() == "PNG"){
        IplImage *ImgPNGTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".png";
        if(cam.bpp == 8){
//            ImgPNGTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgPNGTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgPNGTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgPNGTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgPNGTemp,0);
            cvReleaseImage(&ImgPNGTemp);
        }
    }else{
        IplImage *ImgBMPTemp = cvCreateImage(cvSize(cam.imagew,cam.imageh),8,cam.channels);
        filename = filename + ".bmp";
        if(cam.bpp == 8){
//            ImgBMPTemp->imageData = (char *)cam.ImgDataSave;
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgBMPTemp->imageData[i] = cam.ImgDataSave[i];//(cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgBMPTemp,0);
        }else{
            for (int i = 0; i < cam.imageh * cam.imagew * cam.channels; i++){
                ImgBMPTemp->imageData[i] = (cam.ImgDataSave[2*i + 1] * 256+ cam.ImgDataSave[2*i]) >> 8;
            }
            cvSaveImage(filename.toStdString().c_str(),ImgBMPTemp,0);
            cvReleaseImage(&ImgBMPTemp);
        }
    }

    cam.canTransferData = true;

    ui->liveFormatCbx->setEnabled(true);
    ui->opt_tbs->setText("Save" + ui->liveFormatCbx->currentText() + " format image successfully.");
}

void Guider::on_pushButton_10_clicked()
{
    if(ui->pushButton_10->text() == "Debug ON"){
        EnableQHYCCDMessage(true);
        ui->pushButton_10->setText("Debug OFF");
    }else if(ui->pushButton_10->text() == "Debug OFF"){
        EnableQHYCCDMessage(false);
        ui->pushButton_10->setText("Debug ON");
    }
}
