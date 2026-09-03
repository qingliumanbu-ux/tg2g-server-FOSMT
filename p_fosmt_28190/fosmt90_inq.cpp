/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年8月15日
Description:	炼钢全厂指示数据表查询
Update:
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

/* ***** 静态函数申明 ***** */
CString set_sqlstr_order(CString table_name);

BM2F_ENTERACE(fosmt90_inq)
int f_fosmt90_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_condition = "";
	CString sqlstr_order = "";
	int TotalRecordCount = 0;

	CPageInfo pageInfo;

	CDbCommand cmd_inq(conn);

	CString table_name = "";
	int condi_flag = 0;

	try
	{
		CDateTime datetime = CDateTime::Now();

		//获取分页信息
		try
		{
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			Log::Trace("", __FUNCTION__, "未传入分页信息！");
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}

		//获取数据表名
		table_name = bcls_rec->Tables["TABLE_NAME"].Rows[0]["TABLE_NAME"].ToString();
		CModel model(table_name);
		CString tbl = "A";
		Log::Trace("", __FUNCTION__, "table_name[{0}]", table_name);

		//获取查询条件
		if (bcls_rec->Tables[0].Rows.get_Count() > 0)
		{
			model.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			condi_flag = 1;
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count =
				" SELECT COUNT(1) FROM " + table_name + " " + tbl +
				" WHERE 1 = 1"
				;

			sqlstr =
				" SELECT A.* FROM " + table_name + " " + tbl +
				" WHERE 1 = 1"
				;

			//通用查询条件
			if (condi_flag == 1)
			{
				for (int i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
				{
					CString col = bcls_rec->Tables[0].Columns[i].get_ColumnName();
					if (!model.GetFields().Contains(col)) continue;

					{
						//需要特殊处理的字段
					}
					
					if (model.GetFields()[col].ColumnType == DT_STRING && model[col].ToString().Trim() != "")
					{
						Log::Info("", __FUNCTION__, "col[{0}][{1}][STRING]", col, model[col]);
						sqlstr_condition += " AND " + tbl + "." + col + " = @" + col;
						cmd_inq.Parameters.Set(col, model[col].ToString().Trim());
					}
					else if (model.GetFields()[col].ColumnType == DT_DECIMAL && model[col].ToDecimal() != 0)
					{
						Log::Info("", __FUNCTION__, "col[{0}][{1}][DECIMAL]", col, model[col]);
						sqlstr_condition += " AND " + tbl + "." + col + " = @" + col;
						cmd_inq.Parameters.Set(col, model[col].ToDecimal());
					}
				}
			}

			//组合排序条件
			sqlstr_order = set_sqlstr_order(table_name);

			sqlstr_count = sqlstr_count + sqlstr_condition;
			sqlstr = sqlstr + sqlstr_condition + sqlstr_order;
			break;
		}

		//Log::Info("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
		//Log::Info("", __FUNCTION__, "sqlstr_condition[{0}]", sqlstr_condition);

		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息 
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

		//返回提示栏信息
		CFormattable arguments[] = { TotalRecordCount }; // 定义参数列表的数组
		CMessageFormat::Format(s.msg, "查询到信息[{0}]条。", arguments, 1); //查询到[{0}]条记录。
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments2[] = { ts };
		CMessageFormat::Format(s.sysmsg, "SVC用时[{0}ms]", arguments2, 1);
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;

}

CString set_sqlstr_order(CString table_name)
{
	CString str_order = "";
	if (table_name == "TFOSMT01A")
	{
		str_order = " ORDER BY TO_NUMBER(ITEM_ENAME) DESC";
	}
	if (table_name == "TFOSMT01B")
	{
		str_order = " ORDER BY DATE_TIME DESC";
	}
	if (table_name == "TFOSMT02A")
	{
		str_order = " ORDER BY DEP_NAME, BREAKDN_DATE DESC";
	}

	return str_order;
}