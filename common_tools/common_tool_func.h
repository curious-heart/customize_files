#ifndef COMMON_TOOL_FUNC_H
#define COMMON_TOOL_FUNC_H

#include <QString>
#include <QTextEdit>
#include <QFont>

QString common_tool_get_curr_dt_str();
QString common_tool_get_curr_date_str();
QString common_tool_get_curr_time_str();

bool mkpth_if_not_exists(const QString &pth_str);

#define DEF_SHUTDOWN_WAIT_TIME 3
/*return the shutdown command line.*/
QString shutdown_system(QString reason_str = "", int wait_time = DEF_SHUTDOWN_WAIT_TIME);

#define ROUNDUP_UINT16_TO_10(x) ((quint16)(((quint16)(((x) + 5) / 10)) * 10))
#define ARRAY_COUNT(a) (sizeof((a)) / sizeof((a)[0]))

typedef enum
{
    INT_DATA, FLOAT_DATA,
}common_data_type_enum_t;
class RangeChecker
{
public:
#define EDGE_ITEM(a) a
#define EDGE_LIST \
        EDGE_ITEM(EDGE_INCLUDED),\
        EDGE_ITEM(EDGE_EXCLUDED),\
        EDGE_ITEM(EDGE_INFINITE),\
        EDGE_ITEM(EDGE_COUNT),
    typedef enum
    {
        EDGE_LIST
    }range_edge_enum_t;
#undef EDGE_ITEM
static  const char* range_edge_strs[];

private:
    bool valid;
    double min, max;
    range_edge_enum_t low_edge, up_edge;
    const char* unit_str;
public:
    RangeChecker(double min = -1, double max = 1, const char* unit_str = "",
                 range_edge_enum_t low_edge = EDGE_INCLUDED, range_edge_enum_t up_edge = EDGE_INCLUDED);
public:
    bool range_check(double val);

    void set_min_max(double min_v, double max_v);
    void set_edge(range_edge_enum_t low_e, range_edge_enum_t up_e);
    void set_unit_str(const char* unit_s);
    range_edge_enum_t range_low_edge();
    range_edge_enum_t range_up_edge();
    double range_min();
    double range_max();
    QString range_str(common_data_type_enum_t d_type, double factor = 1, QString new_unit_str = "");
};

void append_str_with_color_and_weight(QTextEdit* ctrl, QString str,
                             QColor new_color = Qt::black,
                             int new_font_weight = -1);

int count_discrete_steps(double low_edge, double up_edge, double step);
#endif // COMMON_TOOL_FUNC_H
