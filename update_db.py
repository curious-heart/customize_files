import sys
import os
import getpass
import pyodbc

usage_str = "usage: " + os.path.basename(__file__) + " src_mdb_file_name dst_mdb_folder [-r]"
if(len(sys.argv) != 3 and len(sys.argv) != 4):
    print(usage_str)
    sys.exit(-1)

rec_flag = False
if(len(sys.argv) == 4 and sys.argv[3] == "-r"):
    rec_flag = True

def rec_data_rows(mdb_fpn, row_list):
    csv_fpn = os.path.splitext(mdb_fpn)[0] + ".csv"
    csv_file = open(csv_fpn, "w", encoding = "utf-8")
    for row in row_list:
        print(row, file = csv_file)
    csv_file.close()

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

src_mdb_fn, dst_mdb_folder = sys.argv[1], sys.argv[2]
src_dbq_v = os.path.abspath(src_mdb_fn)

src_conn_str = "DRIVER=" + driver_str + ";" \
             + "DBQ=" + src_dbq_v + ";" \
             + "PWD=" + pwd_str

src_cnxn = pyodbc.connect(src_conn_str)
src_crsr = src_cnxn.cursor()
sql_draw_from_src = sql_draw_cared_data 
src_crsr.execute(sql_draw_from_src)
src_data_rows = src_crsr.fetchall()
if rec_flag:
    rec_data_rows(src_dbq_v, src_data_rows)

dst_mdb_abs_pth = os.path.abspath(dst_mdb_folder)
dst_mdb_walkout = os.walk(dst_mdb_abs_pth)
for pth, _, fn_list in dst_mdb_walkout:
    for fn in fn_list:
        if not(fn.endswith(".mdb")): continue
        dst_dbq_v = pth + "\\" + fn
        print("\n\ndst file: " + dst_dbq_v)
        dst_conn_str = "DRIVER=" + driver_str + ";" \
                     + "DBQ=" +  dst_dbq_v + ";" \
                     + "PWD=" + pwd_str
        dst_cnxn = pyodbc.connect(dst_conn_str)
        dst_crsr = dst_cnxn.cursor()
        for src_data_row in src_data_rows:
            ID_v = src_data_row[0]
            KV_v = src_data_row[2]
            uA_v = src_data_row[3]
            uS_v = src_data_row[4]
            uAS_v = src_data_row[5]
            sql_upd_dst = "update " + tbl_name + " set " \
                         + db_col_s_KV + "=" + str(KV_v) + ", " + db_col_s_uA + "=" + str(uA_v) + ", " \
                         + db_col_s_uS + "=" + str(uS_v) + ", " + db_col_s_uAS + "=" + str(uAS_v) \
                         + " where " + db_col_s_ID + "=" + str(ID_v)
            dst_crsr.execute(sql_upd_dst)
        dst_crsr.commit()
        if(rec_flag):
            dst_crsr.execute(sql_draw_cared_data)
            dst_new_data_rows = dst_crsr.fetchall()
            rec_data_rows(dst_dbq_v, dst_new_data_rows)
        dst_crsr.close()
        dst_cnxn.close()


"""
for table_info in src_crsr.tables(tableType='TABLE'):
    print(table_info.table_name)
print("--------------------")
"""
