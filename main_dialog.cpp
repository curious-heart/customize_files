#include "main_dialog.h"
#include "ui_main_dialog.h"

#include "common_tools/common_tool_func.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QColor>
#include <QSettings>

static const char* gs_str_configs_file_pn = "configs/configs.ini";
static const char* gs_str_config_sec_def_locs = "def_locs";
static const char* gs_str_config_key_def_cfg_file = "def_cfg_file";
static const char* gs_str_config_key_def_src_loc = "def_src_loc";

main_dialog::main_dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::main_dialog)
{
    ui->setupUi(this);

    connect(&m_files_customizer, &FilesCustomizer::log_str_for_user_sig,
            this, &main_dialog::log_str_for_user_sig_handler);

    read_configs();
}

main_dialog::~main_dialog()
{
    delete ui;
}

void main_dialog::read_configs()
{
    QString def_str;
    QFileInfo f_info;
    QSettings configs(gs_str_configs_file_pn, QSettings::IniFormat);

    configs.beginGroup(gs_str_config_sec_def_locs);

    def_str = configs.value(gs_str_config_key_def_cfg_file, QString("")).toString();
    f_info.setFile(def_str);
    if(f_info.exists() && f_info.isFile())
    {
        m_files_customizer.m_cfg_file_fpn = def_str;
        ui->selCfgFileLE->setText(m_files_customizer.m_cfg_file_fpn);
    }

    def_str = configs.value(gs_str_config_key_def_src_loc, QString("")).toString();
    f_info.setFile(def_str);
    if(f_info.exists() && f_info.isDir())
    {
        m_files_customizer.m_src_folder_fpn = def_str;
        ui->selSrcFolderLE->setText(m_files_customizer.m_src_folder_fpn);
    }

    configs.endGroup();
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
    if(QFileInfo(m_files_customizer.m_cfg_file_fpn).isFile()
        && QFileInfo(m_files_customizer.m_src_folder_fpn).isDir()
        && QFileInfo(m_files_customizer.m_dst_folder_fpn).isDir())
    {
        m_files_customizer.process();
    }
    else
    {
        QMessageBox::information(this, "", tr("请检查配置文件、源文件夹、目标文件夹是否存在！"));
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
    DIY_LOG(lvl, str);
}

void main_dialog::on_clearMsgPB_clicked()
{
    ui->logTE->clear();
}

void main_dialog::on_tttPB_clicked()
{
    dir_walkout_ret_t walkout_ret;

    if(!m_files_customizer.m_dst_folder_fpn.isEmpty())
    {
        /*
        walkout_dir(m_files_customizer.m_dst_folder_fpn, QStringList(), walkout_ret);
        for(int n_idx = 0; n_idx < walkout_ret.size(); ++n_idx)
        {
            ui->logTE->append(walkout_ret[n_idx].curr_pth);
            ui->logTE->append("......................................\n");

            for(int d_idx = 0; d_idx < walkout_ret[n_idx].dir_str_list.size(); ++d_idx)
            {
                ui->logTE->append((walkout_ret[n_idx].dir_str_list[d_idx]));
            }
            ui->logTE->append("......................................\n");

            for(int f_idx = 0; f_idx < walkout_ret[n_idx].file_str_list.size(); ++f_idx)
            {
                ui->logTE->append((walkout_ret[n_idx].file_str_list[f_idx]));
            }

            ui->logTE->append("======================================\n");
        }

        ui->logTE->append("======================================");
        ui->logTE->append("======================================\n");
        */
        QStringList file_list;
        collect_files(m_files_customizer.m_dst_folder_fpn, QStringList(), file_list);
        for(int f_idx = 0; f_idx < file_list.size(); ++ f_idx)
        {
            ui->logTE->append(file_list[f_idx]);
        }
    }
}

