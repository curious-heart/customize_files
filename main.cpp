﻿#include "main_dialog.h"
#include "logger/logger.h"
#include "version_def/version_def.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    if(QT_VERSION>=QT_VERSION_CHECK(5,6,0))
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    main_dialog w;
    QThread th;
    int ret;

    start_log_thread(th);

    QString title_str = QString("%1 %2").arg(a.applicationName(), APP_VER_STR);
    w.setWindowTitle(title_str);

    w.show();
    ret = a.exec();

    end_log_thread(th);

    return ret;
}
