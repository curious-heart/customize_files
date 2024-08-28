import sys
import os
import getpass
import pyodbc

usage_str = "usage: " + os.path.basename(__file__) + " mdb_file_name|dir"
if(len(sys.argv) != 2):
    print(usage_str)
    sys.exit(-1)

def rec_data_rows(mdb_fpn, row_list):
    csv_fpn = os.path.splitext(mdb_fpn)[0] + ".csv"
    csv_file = open(csv_fpn, "w", encoding = "utf-8")
    for row in row_list:
        print(row, file = csv_file)
    csv_file.close()
    return csv_fpn

driver_str = r"{Microsoft Access Driver (*.mdb, *.accdb)}"
tbl_name = "PositionParameter"
db_col_s_ID = "ID"
db_col_s_PosName = "PositionName"
db_col_s_KV = "KV"
db_col_s_uA = "uA"
db_col_s_uS = "uS"
db_col_s_uAS = "uAS"

sql_draw_cared_data = "select " \
                    + db_col_s_ID + ", " + db_col_s_PosName + ", " \
                    + db_col_s_KV + ", " + db_col_s_uA + ", " + db_col_s_uS + ", " + db_col_s_uAS \
                    + " from " + tbl_name

print("please input password:")
pwd_str = getpass.getpass(prompt = "")

src_mdb_fn_list = []
if os.path.isdir(sys.argv[1]):
    dir_walkout = os.walk(sys.argv[1])
    for pth, _, fn_list in dir_walkout:
        for fn in fn_list:
            src_mdb_fn_list.append(pth + "\\" + fn)
else:
    src_mdb_fn_list.append(sys.argv[1])

for src_mdb_fn in src_mdb_fn_list:
    src_dbq_v = os.path.abspath(src_mdb_fn)

    src_conn_str = "DRIVER=" + driver_str + ";" \
                 + "DBQ=" + src_dbq_v + ";" \
                 + "PWD=" + pwd_str

    src_cnxn = pyodbc.connect(src_conn_str)
    src_crsr = src_cnxn.cursor()
    sql_draw_from_src = sql_draw_cared_data 
    src_crsr.execute(sql_draw_from_src)
    src_data_rows = src_crsr.fetchall()
    result_csv_fpn = rec_data_rows(src_dbq_v, src_data_rows)
    src_crsr.close()
    src_cnxn.close()
    print("output data file: " + result_csv_fpn)


