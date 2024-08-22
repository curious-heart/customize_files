﻿#ifndef DIALOG_H
#define DIALOG_H

#include "filescustomizer.h"

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class main_dialog; }
QT_END_NAMESPACE

class main_dialog: public QDialog
{
    Q_OBJECT

public:
    main_dialog(QWidget *parent = nullptr);
    ~main_dialog();

private slots:
    void on_selCfgFilePB_clicked();
    void on_selSrcFolderPB_clicked();
    void on_selDstFolderPB_clicked();
    void on_startProcPB_clicked();

    void log_str_for_user_sig_handler(QString str, LOG_LEVEL lvl = LOG_INFO);

private:
    Ui::main_dialog *ui;
    FilesCustomizer m_files_customizer;
};
#endif // DIALOG_H
