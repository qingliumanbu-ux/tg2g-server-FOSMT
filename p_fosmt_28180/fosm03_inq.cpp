/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年6月28日
Description:	炼钢运行管控工序信息查询
Update:
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

/* ***** 静态函数申明 ***** */
//void get_fosmt00(CDecimal page_id, EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//获取标签名返回块

BM2F_ENTERACE(fosm03_inq)
int f_fosm03_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;
	CModel tfosm03("TFOSM03");
	CModel tfosm03a("TFOSM03A");
	EIClass inBlock_Rules;
	EIClass outBlock_Rules;  //规则表
	EIClass outBlock_Result; //查询结果
	CString sqlstr = "";
	CString sqlstr_rule = "";
	CString sqlstr_desc = "";
	CString sqlstr_where = "";
	CString sqlstr_group = "";
	CString v_table_name = "";
	CString v_table_ename = "";
	CString v_table_name_field = "";
	CString v_mode_code= "";
	CString v_column_name = "";
	CString v_column_cname = "";
	CString v_column_col = "";
	CString v_column_val = "";
	CString v_column_fmt = "";
	CString v_column_name_field = "";
	int blkNum,i, table_rows = 0;
	int bcls_ret_tbrows = 0;
	CDecimal i_row_idx = 0;
	CDecimal i_col_idx = 0;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString datetime_start = "";
	CString datetime_end = "";
	 


	CString sqlstr_count = "";
	CString sqlstr_order = "";
	int	TotalRecordCount = 0;
	CPageInfo pageInfo;
	
	CDecimal page_id = 0;
	CString date_time = "";
	CString widget_id = "";
	CString tbl = "";

	


	try
	{
		CDateTime datetime = CDateTime::Now();

		blkNum = inBlock_Rules.Tables.IndexOf("RUN_RULE");
		if (blkNum < 0)
		{
			//执行规则目
			inBlock_Rules.Tables.Add("RUN_RULE");
		}
		blkNum = inBlock_Rules.Tables.IndexOf("DESC_RULE");
		if (blkNum < 0)
		{
			//执行规则目
			inBlock_Rules.Tables.Add("DESC_RULE");
		}

		//校验传参
		if (!bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "STATION_ID");
			tfosm03["STATION_ID"] = "B";
			//strcpy(s.msg, "未传入STATION_ID");
			//throw CApplicationException(-1, s.msg, log.Location);
		}
		else
		{
			tfosm03["STATION_ID"] = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString();
			Log::Trace("", __FUNCTION__, "DEV_POS[{0}]", tfosm03["STATION_ID"].ToString());
		}

		if (!bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "STATION_NO");
			tfosm03["STATION_NO"] = "1";
			//strcpy(s.msg, "未传入STATION_NO!");
			//throw CApplicationException(-1, s.msg, log.Location);
		}
		else
		{
			tfosm03["STATION_NO"] = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString();
			Log::Trace("", __FUNCTION__, "STATION_NO[{0}]", tfosm03["STATION_NO"].ToString());
		}
	
		if (bcls_rec->Tables[0].Columns.Contains("DEV_POS"))
		{
			tfosm03["DEV_POS"] = bcls_rec->Tables[0].Rows[0]["DEV_POS"].ToString();
			Log::Trace("", __FUNCTION__, "DEV_POS[{0}]", tfosm03["DEV_POS"].ToString());
		}
		else
		{
			tfosm03["DEV_POS"] = " ";
		}

		//获取其他传入参数
		//date_time = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString();
		//page_id = bcls_rec->Tables[0].Rows[0]["PAGE_ID"].ToDecimal();
		//Log::Trace("", __FUNCTION__, "date_time[{0}]page_id[{1}]", date_time, page_id);



		//读取待执行的查询规则集
		sqlstr_rule = "SELECT * FROM TFOSM03 WHERE STATION_ID = @STATION_ID and STATION_NO = @STATION_NO  ";
		//读取前台展示规则集
		sqlstr_desc = "SELECT * FROM TFOSM03a WHERE STATION_ID = @STATION_ID and STATION_NO = @STATION_NO   ";

		if (tfosm03["DEV_POS"].ToString().Trim() != "")
		{
			sqlstr_rule += " AND DEV_POS = @DEV_POS ";
			sqlstr_desc += " AND DEV_POS = @DEV_POS ";
		}
		sqlstr_desc += " order by table_seq,row_seq,col_seq ";


		cmd_inq.SetCommandText(sqlstr_rule);
		Log::Info("", __FUNCTION__, "sqlstr_rule =[{0}]", sqlstr_rule);
		cmd_inq.Parameters.Set("STATION_ID", tfosm03["STATION_ID"].ToString());
		cmd_inq.Parameters.Set("STATION_NO", tfosm03["STATION_NO"].ToString());
		if (tfosm03["DEV_POS"].ToString().Trim() != "")
		{
			cmd_inq.Parameters.Set("DEV_POS", tfosm03["DEV_POS"].ToString());
		}

		cmd_inq.ExecuteQuery(inBlock_Rules.Tables["RUN_RULE"]);//查询逻辑集
		cmd_inq.Close();
		Log::Info("", __FUNCTION__, "RULE_count =[{0}]", inBlock_Rules.Tables["RUN_RULE"].Rows.get_Count());

		
		cmd_inq.SetCommandText(sqlstr_desc);
		cmd_inq.ExecuteQuery(inBlock_Rules.Tables["DESC_RULE"]);
		cmd_inq.Close();
		Log::Info("", __FUNCTION__, "desc_count =[{0}]", inBlock_Rules.Tables["DESC_RULE"].Rows.get_Count());//展示规则集

		
		//逐条执行查询逻辑，压入对应的table
		for (i = 0; i < inBlock_Rules.Tables["RUN_RULE"].Rows.get_Count(); i++)
		{
			sqlstr = "";
			sqlstr_where = "";
			v_table_name = "";
			tfosm03.Reset();

			tfosm03.MergeFrom(inBlock_Rules.Tables["RUN_RULE"].Rows[i]);
			tfosm03.TrimOrBlank();
			Log::Info("", __FUNCTION__, "idx_no =[{0}]", tfosm03["IDX_NO"].ToString());

			//sqlstr = inBlock_Rules.Tables["RUN_RULE"].Rows[i]["BACK_C1"].ToString();
			datetime_start = CDateTime::Now().ToString("yyyyMMddHHmmss");
			v_table_name = tfosm03["TABLE_NAME"].ToString();

			Log::Info("", __FUNCTION__, "v_table_name =[{0}]", v_table_name);

			if (v_table_name_field.Find(v_table_name) >= 0)
			{
				Log::Info("", __FUNCTION__, "v_table_name_field =[{0}]", v_table_name);
				CFormattable arguments[] = { v_table_name };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "块名[{0}]重复。", arguments, 1);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else v_table_name_field += v_table_name;

			//增加数据块
			blkNum = outBlock_Result.Tables.IndexOf(v_table_name);
			if (blkNum < 0)
			{
				//执行规则目
				outBlock_Result.Tables.Add(v_table_name);
				outBlock_Result.Tables[v_table_name].Rows.Clear();
			}

			Log::Info("", __FUNCTION__, "PATTERN_NO =[{0}]", tfosm03["PATTERN_NO"].ToString());
			if (tfosm03["PATTERN_NO"].ToString().Trim() = "1")//SQL
			{
				sqlstr += tfosm03["QUERY_SQL"].ToString().Trim();
				sqlstr += tfosm03["SUB_SYSTEM_SQL"].ToString().Trim();
				sqlstr += tfosm03["RULE_SQL_EXP"].ToString().Trim();


				if (sqlstr.Trim().SubstringNE(0, 6).ToUpper() != "SELECT")
				{
					CFormattable arguments[] = { sqlstr.Trim().SubstringNE(0, 6).ToUpper() };// 定义参数列表的数组
					CMessageFormat::Format(s.msg, "sql语句[{0}]不合法！", arguments, 1);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//where 语句拼接，待完善
				sqlstr_where += "";

				sqlstr += sqlstr_where;


				cmd_sql.SetCommandText(sqlstr);

				//sql_where 赋值
				//cmd_sql.Parameters.Set("STATION_NO", tfosm03["STATION_NO"].ToString());
				Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
				cmd_sql.ExecuteQuery(outBlock_Result.Tables[v_table_name]);
				cmd_sql.Close();
				
				Log::Info("", __FUNCTION__, "outBlock_Result table_name =[{0}]rows-[{1}]", v_table_name, outBlock_Result.Tables[v_table_name].Rows.get_Count());

			}
			else if (tfosm03["PATTERN_NO"].ToString().Trim() = "V")//VIEW
			{

			}

		}
		outBlock_Rules.prt();
		//Log::Info("", __FUNCTION__, "展示");

		//模板展示
		for (i = 0; i < inBlock_Rules.Tables["DESC_RULE"].Rows.get_Count(); i++)
		{
			tfosm03a.Reset();
			tfosm03a.MergeFrom(inBlock_Rules.Tables["DESC_RULE"].Rows[i]);
			tfosm03a.TrimOrBlank();
			v_table_name = tfosm03a["TABLE_NAME"].ToString().Trim();
			v_table_ename = tfosm03a["TABLE_ENAME"].ToString().Trim();
			v_column_name = tfosm03a["COLUMN_NAME"].ToString().Trim();
			v_mode_code = tfosm03a["MODE_CODE"].ToString().Trim();
			i_row_idx = tfosm03a["ROW_SEQ"].ToDecimal();
			i_col_idx = tfosm03a["COL_SEQ"].ToDecimal();
			v_column_cname = "COLUNM_CN_"+v_column_name;	//显示列名
			v_column_col = "COLUNM_CL_" + v_column_name;	//显示颜色
			v_column_val = "COLUNM_VA_" + v_column_name;	//显示值
			Log::Info("", __FUNCTION__, "v_column_name =[{0}]", v_column_name);
			//v_column_cname = tfosm03["COLUMN_CNAME"].ToString().Trim();
			//v_column_col = tfosm03["COLOR_DESC"].ToString().Trim();
			Log::Info("", __FUNCTION__, "v_table_ename =[{0}],COLUMN_NAME= [{1}],column_cnam[{2}],column_col=[{3}]", v_table_name, v_column_name, v_column_cname, v_column_col);
			Log::Info("", __FUNCTION__, "v_mode_code =[{0}]", v_mode_code);
			
			//如果查询结果中没有配置的数据源表，跳出
			if (!outBlock_Result.Tables.Contains(v_table_name))continue;
			//如果查询结果中数据源表无数据，跳出
			if (outBlock_Result.Tables[v_table_name].Rows.get_Count() <= 0)continue;

			blkNum = bcls_ret->Tables.IndexOf(v_table_ename);
			if (blkNum < 0)
			{
				//执行规则目
				bcls_ret->Tables.Add(v_table_ename);
				bcls_ret->Tables[v_table_ename].Rows.Clear();
				bcls_ret_tbrows = 0;
			}
			//判断数据整备模式
			if (v_mode_code.Trim() == "0")
			{
				Log::Info("", __FUNCTION__, "v_mode_code =[{0}]", v_mode_code);

				//直接获取结果集
				bcls_ret->Tables[v_table_ename].Clone(outBlock_Result.Tables[v_table_name]);

				int blkNum1 = bcls_ret->Tables.IndexOf(v_table_ename);

				int blkNum2 = outBlock_Result.Tables.IndexOf(v_table_name);

				//Log::Info("", __FUNCTION__, "blkNum1 =[{0}]，v_table_ename【{1}】", blkNum1, v_table_ename);

				//Log::Info("", __FUNCTION__, "blkNum2 =[{0}]，v_table_ename【{1}】", blkNum2, v_table_name);

				continue;
			}

			//如果查询结果中没有配置的数据源列，跳出
			if (!outBlock_Result.Tables[v_table_name].Columns.Contains(v_column_name))continue;

			if (bcls_ret->Tables[v_table_ename].Rows.get_Count() <= 0)bcls_ret->Tables[v_table_name].Rows.Add();

			//if (bcls_ret->Tables[v_table_name].Rows.get_Count() < i_row_idx)bcls_ret->Tables[v_table_name].Rows.Add();
			Log::Info("", __FUNCTION__, "v_table_ename[{0}]", v_table_ename);
			
			if (i_row_idx > 1 && i_col_idx < 2)
			{
				//COLUNM_CN_;
				//FORMAT_CR   ;
				v_column_fmt = "FORMAT_CR_" + i_row_idx.ToString() + i_col_idx.ToString();
				if (!bcls_ret->Tables[v_table_ename].Columns.Contains(v_column_fmt))
				{
					bcls_ret->Tables[v_table_ename].Columns.Add(DT_STRING, v_column_fmt);
					bcls_ret->Tables[v_table_ename].Rows[0][v_column_fmt] = tfosm03a["ITEM_VALUE"].ToString();
				}
				
			}
			if (v_column_name.SubstringNE(0,7)=="FORMAT_")
			{
				//bcls_ret->Tables[v_table_name].Columns.Add(DT_STRING, v_column_name);
				//bcls_ret->Tables[v_table_name].Rows[0][v_column_name] = tfosm03a["ITEM_VALUE"].ToString();
			}
			else
			{
				//增加显示列
				if (!bcls_ret->Tables[v_table_ename].Columns.Contains(v_column_name))
				{
					bcls_ret->Tables[v_table_ename].Columns.Add(DT_STRING, v_column_cname);
					bcls_ret->Tables[v_table_ename].Columns.Add(DT_STRING, v_column_val);
					bcls_ret->Tables[v_table_ename].Columns.Add(DT_STRING, v_column_col);
				}
				//Log::Info("", __FUNCTION__, "v_table_name[{0}]", v_table_name);
				//Log::Info("", __FUNCTION__, "v_table_ename[{0}]", v_table_ename);
				//Log::Info("", __FUNCTION__, "v_column_cname[{0}]", v_column_cname);
				//Log::Info("", __FUNCTION__, "v_column_val[{0}]", v_column_val);
				//Log::Info("", __FUNCTION__, "v_column_col[{0}]", v_column_col);
				//Log::Info("", __FUNCTION__, "v_column_name[{0}]", v_column_name);

				//获取配置的显示标题
				bcls_ret->Tables[v_table_ename].Rows[0][v_column_cname] = tfosm03a["COLUMN_CNAME"].ToString().Trim();
				//获取配置的显示颜色
				bcls_ret->Tables[v_table_ename].Rows[0][v_column_col] = tfosm03a["COLOR_DESC"].ToString().Trim();
				
				//Log::Info("", __FUNCTION__, "aaaa1 - [{0}]", outBlock_Result.Tables[v_table_name].Rows[0][v_column_name].ToString().Trim());
				//根据配置从对应的数据结果中获取需要显示的项目值
				bcls_ret->Tables[v_table_ename].Rows[0][v_column_val] = outBlock_Result.Tables[v_table_name].Rows[0][v_column_name].ToString().Trim();
				//Log::Info("", __FUNCTION__, "展示[{0}]-[{1}]-[{2}]", tfosm03a["COLUMN_CNAME"].ToString(), tfosm03a["COLOR_DESC"].ToString(), "1");//outBlock_Rules.Tables[v_table_name].Rows[0][v_column_name].ToString()
				//Log::Info("", __FUNCTION__, "返回结果集：[{0}]-[{1}]-[{2}]"
				//	, bcls_ret->Tables[v_table_name].Rows[0][v_column_cname].ToString()
				//	, bcls_ret->Tables[v_table_name].Rows[0][v_column_col].ToString()
				//	, bcls_ret->Tables[v_table_name].Rows[0][v_column_val].ToString());

			}

		}
		
		blkNum = bcls_ret->Tables.IndexOf("QMTS");
		Log::Info("", __FUNCTION__, "QMTSblkNum[{0}]", blkNum);
		//bcls_ret->Tables["MMSMSND"].Clone(bcls_rec->Tables["TMMSM33"]);
		//bcls_ret->Tables.

		//返回提示栏信息
		//CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		//CFormattable arguments[] = { ts };
		//CMessageFormat::Format(s.msg, "数据读取成功！SVC用时[{0}ms]", arguments, 1);
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

void get_fosmt00(CDecimal page_id, EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CString sqlstr = "";
	CDbCommand cmd_inq(conn);

	CString tbl = "TFOSMT00";
	bcls_ret->Tables.Add(tbl);

	switch (conn->DatabaseKind)
	{
	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
	case DB_KIND_MSSQL:				// MS SQL Server数据库
	case DB_KIND_ORACLE:	        // Oracle 数据库
	default:
		sqlstr =
			" SELECT ITEM_ENAME, REMARK, PAGE_ID, PAGE_TYPE"
			" FROM " + tbl + " WHERE PAGE_ID = @PAGE_ID ORDER BY ITEM_ENAME"
			;
		break;
	}
	cmd_inq.Parameters.Set("PAGE_ID", page_id.ToString());
	cmd_inq.SetCommandText(sqlstr);
	cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
	cmd_inq.Close();
}
