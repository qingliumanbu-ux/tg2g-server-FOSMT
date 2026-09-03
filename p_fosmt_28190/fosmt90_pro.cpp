/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2010
Author:			zhouyueqi
Version:		1.0
Date:			2023年8月15日
Description:	炼钢全厂指示数据表维护
Update:
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

/* ***** 静态函数申明 ***** */
bool set_model_format(CString deal_flag, CModel& model, CDbConnection* conn);//特殊处理数据

BM2F_ENTERACE(fosmt90_pro)
int f_fosmt90_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	int	TotalRecordCount = 0;

	CPageInfo pageInfo;

	CDbCommand cmd_inq(conn);

	CString table_name = "";
	CString msg = "";
	int proc_sum_add = 0;
	int proc_sum_upd = 0;
	int proc_sum_del = 0;

	try
	{
		CDateTime datetime = CDateTime::Now();

		//获取数据表名
		table_name = bcls_rec->Tables["TABLE_NAME"].Rows[0]["TABLE_NAME"].ToString();
		CModel model(table_name);

		//处理数据
		if (bcls_rec->Tables.Contains("ADD") && bcls_rec->Tables["ADD"].Rows.get_Count() > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				model["REC_CREATOR"] = s.userid;
				model["REC_CREATE_TIME"] = datetime.ToString("yyyyMMddhhmmss");
				model["REC_REVISOR"] = "";
				model["REC_REVISE_TIME"] = "";
				model.TrimOrBlank();
				if (!set_model_format("ADD", model, conn)) throw CApplicationException(-1, s.msg, log.Location);

				if (model.Query())
				{
					Log::Trace("", __FUNCTION__, "第[{0}]条记录，主键重复，跳过新增！", i + 1);
					continue;
				}
				sqlstr = "INSERT INTO " + table_name;
				proc_sum_add += model.Insert();
			}

			if (proc_sum_add == bcls_rec->Tables["ADD"].Rows.get_Count())
			{
				msg += "新增数据：成功[" + to_string(proc_sum_add) + "]条！\n";
			}
			else
			{
				msg += "新增数据：成功[" + to_string(proc_sum_add) + "]条，" +
					"失败[" + to_string(bcls_rec->Tables["ADD"].Rows.get_Count() - proc_sum_add) + "]条，请确认主键是否存在问题！\n";
			}
		}
		if (bcls_rec->Tables.Contains("UPD") && bcls_rec->Tables["UPD"].Rows.get_Count() > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				model.TrimOrBlank();
				if (!set_model_format("UPD", model, conn)) throw CApplicationException(-1, s.msg, log.Location);

				model["REC_REVISOR"] = s.userid;
				model["REC_REVISE_TIME"] = datetime.ToString("yyyyMMddhhmmss");
				sqlstr = "UPDATE " + table_name + " SET ";
				proc_sum_upd += model.Update("*");
			}

			if (proc_sum_upd == bcls_rec->Tables["UPD"].Rows.get_Count())
			{
				msg += "修改数据：成功[" + to_string(proc_sum_upd) + "]条！\n";
			}
			else
			{
				msg += "修改数据：成功[" + to_string(proc_sum_upd) + "]条，" +
					"失败[" + to_string(bcls_rec->Tables["UPD"].Rows.get_Count() - proc_sum_upd) + "]条，请确认主键是否存在问题！\n";
			}
		}
		if (bcls_rec->Tables.Contains("DEL") && bcls_rec->Tables["DEL"].Rows.get_Count() > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				model.Reset();
				model.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);
				if (!set_model_format("DEL", model, conn)) throw CApplicationException(-1, s.msg, log.Location);

				if (!model.Query())
				{
					Log::Trace("", __FUNCTION__, "第[{0}]条记录不存在，跳过删除！", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM " + table_name;
				proc_sum_del += model.Delete();
			}

			if (proc_sum_del == bcls_rec->Tables["DEL"].Rows.get_Count())
			{
				msg += "删除数据：成功[" + to_string(proc_sum_del) + "]条！\n";
			}
			else
			{
				msg += "删除数据：成功[" + to_string(proc_sum_del) + "]条，" +
					"失败[" + to_string(bcls_rec->Tables["DEL"].Rows.get_Count() - proc_sum_del) + "]条，请确认主键是否存在问题！\n";
			}
		}

		strcpy(s.msg, msg);
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

bool set_model_format(CString deal_flag, CModel& model, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	CString col;
	//Log::Trace("", __FUNCTION__, "[{0}][{1}]", col, model[col]);

	//8位时间格式通用处理
	col = "DATE_TIME";
	if (model.GetFields().Contains(col))
	{
		model[col] = model[col].ToString().SubstringNE(0, 8);
	}

	col = "BREAKDN_DATE";
	if (model.GetFields().Contains(col))
	{
		model[col] = model[col].ToString().SubstringNE(0, 8);
	}
	
	//数据表特殊处理
	if (model.GetTableName() == "TFOSMT01A" && deal_flag == "ADD")
	{
		CString sqlstr = "";
		CDbCommand cmd_inq(conn);

		col = "ITEM_ENAME";
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = " SELECT MAX(TO_NUMBER(ITEM_ENAME)) FROM TFOSMT01A WHERE ITEM_ENAME <> ' '";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			model[col] = cmd_inq.GetDecimal(1) + 1;
		}
		else
		{
			model[col] = 1;
		}
		cmd_inq.Close();
	}
	if (model.GetTableName() == "TFOSMT01B" && deal_flag == "ADD")
	{
		//校验主键
		col = "DATE_TIME";
		if (model[col].ToString().Trim() == "")
		{
			strcpy(s.msg, "日期时间不能为空！");
			return false;
		}
	}
	if (model.GetTableName() == "TFOSMT02A" && deal_flag == "ADD")
	{
		//校验主键
		col = "DEP_NAME";
		if (model[col].ToString().Trim() == "")
		{
			strcpy(s.msg, "部门名称不能为空！");
			return false;
		}
		col = "BREAKDN_DATE";
		if (model[col].ToString().Trim() == "")
		{
			strcpy(s.msg, "故障日期不能为空！");
			return false;
		}

		CString sqlstr = "";
		CDbCommand cmd_inq(conn);
		
		//获取当前周期
		col = "OPERATE_CYCLE";
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = 
				" SELECT DAYS(TO_DATE(@BREAKDN_DATE,'YYYYMMDD')) - DAYS(TO_DATE(MAX(BREAKDN_DATE),'YYYYMMDD'))"
				" FROM TFOSMT02A"
				" WHERE DEP_NAME = @DEP_NAME";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BREAKDN_DATE", model["BREAKDN_DATE"].ToString());
		cmd_inq.Parameters.Set("DEP_NAME", model["DEP_NAME"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			model[col] = cmd_inq.GetDecimal(1);
		}
		else
		{
			model[col] = 0;
		}
		cmd_inq.Close();

		//获取最长周期
		col = "TOTAL_OPERATE_CYCLE";
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr =
				" SELECT MAX(TOTAL_OPERATE_CYCLE)"
				" FROM TFOSMT02A"
				" WHERE DEP_NAME = @DEP_NAME";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("BREAKDN_DATE", model["BREAKDN_DATE"].ToString());
		cmd_inq.Parameters.Set("DEP_NAME", model["DEP_NAME"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (model["OPERATE_CYCLE"].ToDecimal() > cmd_inq.GetDecimal(1))
			{
				model[col] = model["OPERATE_CYCLE"].ToDecimal();
			}
			else
			{
				model[col] = cmd_inq.GetDecimal(1);
			}
		}
		else
		{
			model[col] = 0;
		}
		cmd_inq.Close();

	}
	if (model.GetTableName() == "TFOSMT03B" && deal_flag == "ADD")
	{
		//校验主键
		col = "DEP_NAME";
		if (model[col].ToString().Trim() == "")
		{
			strcpy(s.msg, "部门名称不能为空！");
			return false;
		}
		col = "DATE_TIME";
		if (model[col].ToString().Trim() == "")
		{
			strcpy(s.msg, "日期时间不能为空！");
			return false;
		}
	}
	return true;
}
