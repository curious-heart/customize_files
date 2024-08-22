#ifndef FILESCUSTOMIZER_H
#define FILESCUSTOMIZER_H

#include "logger/logger.h"

#include <QObject>
#include <QSettings>

class FilesCustomizer : public QObject
{
    Q_OBJECT
public:
    explicit FilesCustomizer(QObject *parent = nullptr);

signals:
    void log_str_for_user_sig(QString str, LOG_LEVEL lvl = LOG_INFO);

private:
    bool overwrite_files(QSettings &cfg);

public:
    QString m_cfg_file_fpn, m_src_folder_fpn, m_dst_folder_fpn;
    void process();
};

#endif // FILESCUSTOMIZER_H
