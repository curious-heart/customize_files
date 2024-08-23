#include "main_dialog.h"
#include "ui_main_dialog.h"

#include "common_tools/common_tool_func.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QColor>

main_dialog::main_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::main_dialog)
{
    ui->setupUi(this);

    connect(&m_files_customizer, &FilesCustomizer::log_str_for_user_sig,
            this, &main_dialog::log_str_for_user_sig_handler);
}

main_dialog::~main_dialog()
{
    delete ui;
}

void main_dialog::on_selCfgFilePB_clicked()
{
    static const char* ls_cfg_file_filter = "Config File(*.ini)";
    static const char* ls_sel_cfg_dlg_title = "选择配置文件";
    QString fpn =  QFileDialog::getOpenFileName(this,  tr(ls_sel_cfg_dlg_title),
                                                  ".", ls_cfg_file_filter);
    if(!fpn.isEmpty())
    {
        ui->selCfgFileLE->setText(fpn);
        m_files_customizer.m_cfg_file_fpn = fpn;
    }
}

void main_dialog::on_selSrcFolderPB_clicked()
{
    static const char* ls_sel_src_folder_title = "选择源文件夹";
    QString fpn = QFileDialog::getExistingDirectory(this, tr(ls_sel_src_folder_title), ".");

    if(!fpn.isEmpty())
    {
        ui->selSrcFolderLE->setText(fpn);
        m_files_customizer.m_src_folder_fpn = fpn;
    }
}

void main_dialog::on_selDstFolderPB_clicked()
{
    static const char* ls_sel_dst_folder_title = "选择目标文件夹";
    QString fpn = QFileDialog::getExistingDirectory(this, tr(ls_sel_dst_folder_title), ".");

    if(!fpn.isEmpty())
    {
        ui->selDstFolderLE->setText(fpn);
        m_files_customizer.m_dst_folder_fpn = fpn;
    }
}


void main_dialog::on_startProcPB_clicked()
{
    if(!m_files_customizer.m_cfg_file_fpn.isEmpty()
            && !m_files_customizer.m_src_folder_fpn.isEmpty()
            && !m_files_customizer.m_dst_folder_fpn.isEmpty())
    {
        m_files_customizer.process();
    }
    else
    {
        QMessageBox::information(this, "", tr("请首先选择配置文件，并指定源文件夹和目的文件夹"));
    }
}

void main_dialog::log_str_for_user_sig_handler(QString str, LOG_LEVEL lvl, QColor spec_color)
{
    QColor new_color;

    if(spec_color.isValid())
    {
        new_color = spec_color;
    }
    else
    {
        switch(lvl)
        {
        case LOG_DEBUG:
            new_color = Qt::gray;
            break;

        case LOG_INFO:
            new_color = ui->logTE->textColor();
            break;

        case LOG_WARN:
            new_color = Qt::darkYellow;
            break;

        case LOG_ERROR:
        default:
            new_color = Qt::red;
            break;
        }
    }

    append_str_with_color_and_weight(ui->logTE, str, new_color);
}
