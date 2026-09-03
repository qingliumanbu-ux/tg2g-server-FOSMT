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

BM2F_ENTERACE(fosm02_inq)
int f_fosm02_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_where = "";
	CString sqlstr_order = "";
	int	TotalRecordCount = 0;

	CPageInfo pageInfo;
	CDbCommand cmd_inq(conn);

	CDecimal page_id = 0;
	CString date_time = "";
	CString widget_id = "";
	CString tbl = "";

	CModel tfosmt00("TFOSMT00");

	try
	{
		CDateTime datetime = CDateTime::Now();

		//校验传参
		if (!bcls_rec->Tables[0].Columns.Contains("DATE_TIME"))
		{
			strcpy(s.msg, "未传入DATE_TIME");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (!bcls_rec->Tables[0].Columns.Contains("PAGE_ID"))
		{
			strcpy(s.msg, "未传入PAGE_ID!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables[0].Columns.Contains("WIDGET_ID"))
		{
			widget_id = bcls_rec->Tables[0].Rows[0]["WIDGET_ID"].ToString();
			Log::Trace("", __FUNCTION__, "widget_id[{0}]", widget_id);
		}

		//获取传入参数
		date_time = bcls_rec->Tables[0].Rows[0]["DATE_TIME"].ToString();
		page_id = bcls_rec->Tables[0].Rows[0]["PAGE_ID"].ToDecimal();
		Log::Trace("", __FUNCTION__, "date_time[{0}]page_id[{1}]", date_time, page_id);

		//模板
		if (false)
		{
			tbl = "";
			{
				bcls_ret->Tables.Add(tbl);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr =
						""
						;
					break;
				}
				cmd_inq.Parameters.Set("DATE_TIME", date_time);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
				cmd_inq.Close();
			}
		}

		if (page_id == 0)
		{
			//页面标题标签及微件清单
			tbl = "TFOSMT00";
			bcls_ret->Tables.Add(tbl);

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr =
					" SELECT ITEM_ENAME, ITEM_CNAME PAGE_NAME, REMARK, PAGE_ID, PAGE_TYPE, REMARK_1 WIDGET_LIST"
					" FROM " + tbl + " WHERE 1 = 1 ORDER BY PAGE_TYPE, ITEM_ENAME"
					;
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
			cmd_inq.Close();
		}
		else
		{
			get_fosmt00(page_id, bcls_rec, bcls_ret, conn);

			if (page_id == 1)
			{
				//安全与环保
				//TODO:从TFOSMT01A筛选每日警句，写入TFOSMT01C
				CModel tfosmt01c("TFOSMT01C");
				tfosmt01c["DATE_TIME"] = date_time;

				if (tfosmt01c.QueryCount("DATE_TIME") <= 0)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT * FROM TFOSMT01A WHERE 1 = 1 ORDER BY RAND() LIMIT 1"
							;
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						cmd_inq.Fetch(tfosmt01c);
						tfosmt01c["DATE_TIME"] = date_time;
						tfosmt01c["REC_CREATOR"] = s.userid;
						tfosmt01c["REC_CREATE_TIME"] = s.datetime;
						tfosmt01c["REC_REVISOR"] = " ";
						tfosmt01c["REC_REVISE_TIME"] = " ";
						tfosmt01c.TrimOrBlank();
						tfosmt01c.Insert();
					}
					cmd_inq.Close();
				}

				//读取数据
				tbl = "TFOSMT01";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT 'LABEL01-1' ITEM_ENAME, REMARK FROM TFOSMT01C WHERE DATE_TIME = @DATE_TIME"
							" UNION"
							" SELECT 'LABEL01-2' ITEM_ENAME, REMARK FROM TFOSMT01B WHERE DATE_TIME = @DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 2)
			{
				//稳定周期
				tbl = "TFOSMT02";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.CODE, A.CODE_DESC_1_CONTENT DEP_NAME, C.BREAKDN_DATE, C.REMARK REMARK_BFR, B.REMARK,"
							" DECODE(B.BREAKDN_DATE, NULL, DAYS(TO_DATE(@DATE_TIME,'YYYYMMDD')) - DAYS(TO_DATE(C.BREAKDN_DATE,'YYYYMMDD')), 0) OPERATE_CYCLE,"
							" MAX(C.TOTAL_OPERATE_CYCLE, DECODE(B.REMARK, NULL, DAYS(TO_DATE(@DATE_TIME,'YYYYMMDD')) - DAYS(TO_DATE(C.BREAKDN_DATE,'YYYYMMDD')), B.TOTAL_OPERATE_CYCLE)) TOTAL_OPERATE_CYCLE"
							" FROM TEP0002 A"
							" LEFT JOIN TFOSMT02A B ON A.CODE = B.DEP_NAME AND B.BREAKDN_DATE = @DATE_TIME"
							" LEFT JOIN"
							" (SELECT DEP_NAME, REMARK, BREAKDN_DATE, TOTAL_OPERATE_CYCLE FROM TFOSMT02A"
							" WHERE (DEP_NAME, BREAKDN_DATE) IN"
							" (SELECT DEP_NAME, MAX(BREAKDN_DATE) FROM TFOSMT02A WHERE BREAKDN_DATE < @DATE_TIME GROUP BY DEP_NAME)"
							" ) C ON A.CODE = C.DEP_NAME"
							" LEFT JOIN TFOSMT02B D ON A.CODE = D.DEP_NAME"
							" WHERE A.CODE_CLASS = 'FOSMT1'"
							" AND A.CODE_DESC_2_CONTENT != ' '"
							" ORDER BY A.CODE_DESC_2_CONTENT"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 3)
			{
				//月度生产
				//01本月生产情况
				tbl = "TFOSMT03A";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT"
							" SUBSTR(A.DATE_TIME,7) DATE_TIME,"
							" A.PLAN_CHARGE PLAN_CHARGE_1, A.PRODUCT_CHARGE PRODUCT_CHARGE_1, CAST(A.SLAB_WT AS DECIMAL(10,2)) SLAB_WT_1,"
							" A.CC_REMAIN_NUM CC_REMAIN_NUM_1, A.CC_REMAIN_RATE CC_REMAIN_RATE_1,"
							" B.PLAN_CHARGE PLAN_CHARGE_2, B.PRODUCT_CHARGE PRODUCT_CHARGE_2, CAST(B.SLAB_WT AS DECIMAL(10,2)) SLAB_WT_2,"
							" B.CC_REMAIN_NUM CC_REMAIN_NUM_2, B.CC_REMAIN_RATE CC_REMAIN_RATE_2,"
							" CAST(A.SLAB_WT + B.SLAB_WT AS DECIMAL(10,2)) TOTAL_SLAB_WT,"
							" CAST(ROUND(DECODE(A.SLAB_WT + B.SLAB_WT, 0, 0, B.SLAB_WT / (A.SLAB_WT + B.SLAB_WT)), 2) AS DECIMAL(5,2)) PRODUCT_RATE,"
							" CASE WHEN A.PLAN_CHARGE > 0 THEN 1 ELSE 0 END PLAN_PROD_DAY,"
							" CASE WHEN A.PRODUCT_CHARGE > 0 THEN 1 ELSE 0 END REAL_PRODUCT_DAY,"
							" CAST(DAY(TO_DATE(A.DATE_TIME,'YYYYMMDD')) / TO_NUMBER(TO_CHAR(LAST_DAY(TO_DATE(A.DATE_TIME,'YYYYMMDD')), 'DD')) * 100 AS DECIMAL(5,2)) TIME_PROGRESS,"
							" CAST(DECODE(C.PLAN_SLAB_WT_1, 0, 0, A.ACCUM_SLAB_WT / C.PLAN_SLAB_WT_1) * 100 AS DECIMAL(4,1)) PROD_PROGRESS_1,"
							" CAST(DECODE(C.PLAN_SLAB_WT_2, 0, 0, B.ACCUM_SLAB_WT / C.PLAN_SLAB_WT_2) * 100 AS DECIMAL(4,1)) PROD_PROGRESS_2,"
							" CAST(DECODE(C.PLAN_SLAB_WT, 0, 0, (A.ACCUM_SLAB_WT + B.ACCUM_SLAB_WT) / C.PLAN_SLAB_WT) * 100 AS DECIMAL(4,1)) PROD_PROGRESS,"
							" C.PLAN_SLAB_WT, C.PLAN_SLAB_WT_1, C.PLAN_SLAB_WT_2"
							" FROM TFOSMT03A A"
							" LEFT JOIN TFOSMT03A B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
							" LEFT JOIN"
							" (SELECT A.MONTH, A.PLAN_SLAB_WT PLAN_SLAB_WT_1, B.PLAN_SLAB_WT PLAN_SLAB_WT_2,"
							" A.PLAN_SLAB_WT + B.PLAN_SLAB_WT PLAN_SLAB_WT"
							" FROM TFOSMT03C A"
							" LEFT JOIN TFOSMT03C B ON A.MONTH = B.MONTH AND B.FACTORY_DIV = 'A20'"
							" WHERE A.FACTORY_DIV = 'A10' AND A.MONTH = SUBSTR(@DATE_TIME, 1, 6)) C ON 1 = 1"
							" WHERE A.FACTORY_DIV = 'A10' AND A.DATE_TIME <= @DATE_TIME AND A.DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "PLAN_CHARGE_1", "PRODUCT_CHARGE_1", "SLAB_WT_1", "CC_REMAIN_NUM_1",
												"PLAN_CHARGE_2", "PRODUCT_CHARGE_2", "SLAB_WT_2", "CC_REMAIN_NUM_2",
												"TOTAL_SLAB_WT", "PLAN_PROD_DAY", "REAL_PRODUCT_DAY" };
					vector<CString> col_avg = { "CC_REMAIN_RATE_1", "CC_REMAIN_RATE_2", "PRODUCT_RATE" };
					vector<CString> col_last = { "TIME_PROGRESS", "PROD_PROGRESS", "PROD_PROGRESS_1", "PROD_PROGRESS_2",
												 "PLAN_SLAB_WT", "PLAN_SLAB_WT_1", "PLAN_SLAB_WT_2" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0: row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}
						//取最后
						if (i == count - 1)
						{
							for (int j = 0; j < col_last.size(); j++)
							{
								row[col_last[j]] = bcls_ret->Tables[tbl].Rows[i][col_last[j]].ToDecimal();
							}
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count).Round(1);
					}
				}

				//（1）二炼钢生产计划与实绩炉数
				tbl = "TFOSMT03B";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, PLAN_CHARGE, PRODUCT_CHARGE FROM TFOSMT03A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//（2）板坯库存趋势图
				tbl = "TFOSMT03C";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 6 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD'))"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.STOCK_TOTAL_WT STOCK_TOTAL_WT_1, B2.STOCK_TOTAL_WT STOCK_TOTAL_WT_2"
							" FROM A"
							" LEFT JOIN TFOSMT03A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10'"
							" LEFT JOIN TFOSMT03A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A20'"
							" WHERE 1 = 1"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//（3）铸余比例图
				tbl = "TFOSMT03D";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 6 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD'))"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.CC_REMAIN_RATE / 100 CC_REMAIN_RATE_1, B2.CC_REMAIN_RATE / 100 CC_REMAIN_RATE_2"
							" FROM A"
							" LEFT JOIN TFOSMT03A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10'"
							" LEFT JOIN TFOSMT03A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A20'"
							" WHERE 1 = 1"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//02一二炼钢生产情况
				tbl = "TFOSMT03E1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, SMELT_CHARGE, DIR_CHARGE_RATE, SHALLOW_CHARGE_RATE, TOL_BOF_STEEL_WT,"
							" AVG_STEEL_WT, AVG_SLAB_WT, AVG_BOF_IRON_WT, AVG_TPD_IRON_WT, IRON_SLAB_RATE / 100 IRON_SLAB_RATE, MOLTIRON_WT"
							" FROM TFOSMT03A"
							" WHERE 1 = 1"
							" AND DATE_TIME <= @DATE_TIME AND DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							" AND FACTORY_DIV = 'A10'"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "SMELT_CHARGE", "TOL_BOF_STEEL_WT", "MOLTIRON_WT" };
					vector<CString> col_avg = { "DIR_CHARGE_RATE", "SHALLOW_CHARGE_RATE", "AVG_STEEL_WT",
												"AVG_BOF_IRON_WT", "AVG_TPD_IRON_WT", "IRON_SLAB_RATE",
												"AVG_SLAB_WT" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (col_avg[j] == "IRON_SLAB_RATE")
						{
							if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count).Round(3);
						}
						else
						{
							if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count).Round(1);
						}
					}
				}
				tbl = "TFOSMT03E2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, SMELT_CHARGE, DIR_CHARGE_RATE, SHALLOW_CHARGE_RATE, TOL_BOF_STEEL_WT,"
							" AVG_STEEL_WT, AVG_SLAB_WT, AVG_BOF_IRON_WT, AVG_TPD_IRON_WT, IRON_SLAB_RATE / 100 IRON_SLAB_RATE, MOLTIRON_WT"
							" FROM TFOSMT03A"
							" WHERE 1 = 1"
							" AND DATE_TIME <= @DATE_TIME AND DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							" AND FACTORY_DIV = 'A20'"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "SMELT_CHARGE", "TOL_BOF_STEEL_WT", "MOLTIRON_WT" };
					vector<CString> col_avg = { "DIR_CHARGE_RATE", "SHALLOW_CHARGE_RATE", "AVG_STEEL_WT",
												"AVG_BOF_IRON_WT", "AVG_TPD_IRON_WT", "IRON_SLAB_RATE",
												"AVG_SLAB_WT" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (col_avg[j] == "IRON_SLAB_RATE")
						{
							if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count).Round(3);
						}
						else
						{
							if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count).Round(1);
						}
					}
				}

				//铁坯比
				tbl = "TFOSMT03F1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, IRON_SLAB_RATE IRON_SLAB_RATE"
							" FROM TFOSMT03A"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" AND FACTORY_DIV = 'A10'"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT03F2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, IRON_SLAB_RATE IRON_SLAB_RATE"
							" FROM TFOSMT03A"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" AND FACTORY_DIV = 'A20'"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//03放铁情况
				tbl = "TFOSMT03G";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 9 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD'))"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
							" B1.IRON_RELEASE_WT IRON_RELEASE_WT_1, B1.SEQ_NO DEDUCT_PT_1,"
							" B2.IRON_RELEASE_WT IRON_RELEASE_WT_2, B2.SEQ_NO DEDUCT_PT_2,"
							" B3.IRON_RELEASE_WT IRON_RELEASE_WT_3, B3.SEQ_NO DEDUCT_PT_3,"
							" B4.IRON_RELEASE_WT IRON_RELEASE_WT_4, B4.SEQ_NO DEDUCT_PT_4,"
							" B5.IRON_RELEASE_WT IRON_RELEASE_WT_5, B5.SEQ_NO DEDUCT_PT_5,"
							" B6.IRON_RELEASE_WT IRON_RELEASE_WT_6, B6.SEQ_NO DEDUCT_PT_6,"
							" B7.IRON_RELEASE_WT IRON_RELEASE_WT_7, B7.SEQ_NO DEDUCT_PT_7,"
							" DECODE(B1.REMARK, NULL, '', TRIM(B1.REMARK)) ||"
							" DECODE(B2.REMARK, NULL, '', TRIM(B2.REMARK)) ||"
							" DECODE(B3.REMARK, NULL, '', TRIM(B3.REMARK)) ||"
							" DECODE(B4.REMARK, NULL, '', TRIM(B4.REMARK)) ||"
							" DECODE(B5.REMARK, NULL, '', TRIM(B5.REMARK)) ||"
							" DECODE(B6.REMARK, NULL, '', TRIM(B6.REMARK)) ||"
							" DECODE(B7.REMARK, NULL, '', TRIM(B7.REMARK))"
							" REMARK"
							" FROM A"
							" LEFT JOIN TFOSMT03B B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.DEP_NAME = '02'"
							" LEFT JOIN TFOSMT03B B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.DEP_NAME = '03'"
							" LEFT JOIN TFOSMT03B B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.DEP_NAME = '05'"
							" LEFT JOIN TFOSMT03B B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.DEP_NAME = '04'"
							" LEFT JOIN TFOSMT03B B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.DEP_NAME = '01'"
							" LEFT JOIN TFOSMT03B B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.DEP_NAME = '07'"
							" LEFT JOIN TFOSMT03B B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.DEP_NAME = '10'"
							" WHERE 1 = 1"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.ExecuteReader();
					CDecimal iron_release_wt_1;
					CDecimal iron_release_wt_2;
					CDecimal iron_release_wt_3;
					CDecimal iron_release_wt_4;
					CDecimal iron_release_wt_5;
					CDecimal iron_release_wt_6;
					CDecimal iron_release_wt_7;
					CDecimal deduct_pt_1;
					CDecimal deduct_pt_2;
					CDecimal deduct_pt_3;
					CDecimal deduct_pt_4;
					CDecimal deduct_pt_5;
					CDecimal deduct_pt_6;
					CDecimal deduct_pt_7;
					while (cmd_inq.Read())
					{
						iron_release_wt_1 = iron_release_wt_1 + cmd_inq.GetDecimal(2);
						iron_release_wt_2 = iron_release_wt_2 + cmd_inq.GetDecimal(4);
						iron_release_wt_3 = iron_release_wt_3 + cmd_inq.GetDecimal(6);
						iron_release_wt_4 = iron_release_wt_4 + cmd_inq.GetDecimal(8);
						iron_release_wt_5 = iron_release_wt_5 + cmd_inq.GetDecimal(10);
						iron_release_wt_6 = iron_release_wt_6 + cmd_inq.GetDecimal(12);
						iron_release_wt_7 = iron_release_wt_7 + cmd_inq.GetDecimal(14);

						deduct_pt_1 = deduct_pt_1 + cmd_inq.GetDecimal(3);
						deduct_pt_2 = deduct_pt_2 + cmd_inq.GetDecimal(5);
						deduct_pt_3 = deduct_pt_3 + cmd_inq.GetDecimal(7);
						deduct_pt_4 = deduct_pt_4 + cmd_inq.GetDecimal(9);
						deduct_pt_5 = deduct_pt_5 + cmd_inq.GetDecimal(11);
						deduct_pt_6 = deduct_pt_6 + cmd_inq.GetDecimal(13);
						deduct_pt_7 = deduct_pt_7 + cmd_inq.GetDecimal(15);
					}
					cmd_inq.Close();

					CDataRow & row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					row["IRON_RELEASE_WT_1"] = iron_release_wt_1;
					row["IRON_RELEASE_WT_2"] = iron_release_wt_2;
					row["IRON_RELEASE_WT_3"] = iron_release_wt_3;
					row["IRON_RELEASE_WT_4"] = iron_release_wt_4;
					row["IRON_RELEASE_WT_5"] = iron_release_wt_5;
					row["IRON_RELEASE_WT_6"] = iron_release_wt_6;
					row["IRON_RELEASE_WT_7"] = iron_release_wt_7;
					row["DEDUCT_PT_1"] = deduct_pt_1;
					row["DEDUCT_PT_2"] = deduct_pt_2;
					row["DEDUCT_PT_3"] = deduct_pt_3;
					row["DEDUCT_PT_4"] = deduct_pt_4;
					row["DEDUCT_PT_5"] = deduct_pt_5;
					row["DEDUCT_PT_6"] = deduct_pt_6;
					row["DEDUCT_PT_7"] = deduct_pt_7;
				}
			}
			else if (page_id == 4)
			{
				//01生产调整
				tbl = "TFOSMT04A1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" DECODE(B.START_TIME, ' ', ' ', (TO_CHAR(TO_DATE(B.START_TIME, 'YYYYMMDDHH24MISS'), 'HH24:MI')) || '-' ||"
							" DECODE(B.START_TIME, ' ', ' ', TO_CHAR(TO_DATE(B.END_TIME, 'YYYYMMDDHH24MISS'), 'HH24:MI'))) PERIOD_TIME,"
							" B.REMARK, B.STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT04A2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.FACTORY_DIV, A.DATE_TIME, A.SHIFT_NO, A.SHIFT_GROUP,"
							" A.PLAN_CHARGE, A.SMELT_CHARGE, A.PRODUCT_CHARGE, B.ADJUST_CHARGE,"
							" DECODE(B.START_TIME, ' ', ' ', (TO_CHAR(TO_DATE(B.START_TIME, 'YYYYMMDDHH24MISS'), 'HH24:MI')) || '-' ||"
							" DECODE(B.START_TIME, ' ', ' ', TO_CHAR(TO_DATE(B.END_TIME, 'YYYYMMDDHH24MISS'), 'HH24:MI'))) PERIOD_TIME,"
							" B.REMARK, B.STOP_TOTAL_TIME, B.DEP_NAME"
							" FROM TFOSMT04A A"
							" LEFT JOIN TFOSMT04B B ON A.FACTORY_DIV = B.FACTORY_DIV AND A.DATE_TIME = B.DATE_TIME AND A.SHIFT_NO = B.SHIFT_NO"
							" WHERE A.FACTORY_DIV = 'A20'"
							" AND ((A.DATE_TIME = @DATE_TIME AND A.SHIFT_NO = '1')"
							" OR A.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAY,'YYYYMMDD') AND A.SHIFT_NO <> '1')"
							" ORDER BY A.DATE_TIME, A.SHIFT_NO, B.CHARGE_NO"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 5)
			{
				//01保留情况
				tbl = "TFOSMT05A1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT *"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" ORDER BY DATE_TIME, SEQ_NO"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT05A2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT *"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//保留炉数跟踪（每日）
				tbl = "TFOSMT05B1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, COUNT(HEAT_NO) COUNT_PONO"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" GROUP BY DATE_TIME"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT05B2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, COUNT(HEAT_NO) COUNT_PONO"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" GROUP BY DATE_TIME"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//保留元素炉数统计（月度）
				tbl = "TFOSMT05C1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 30 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" GROUP BY ELM_DESC"
							" ORDER BY ELM_DESC"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT05C2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT ELM_DESC, COUNT(ELM_DESC) COUNT_PONO"
							" FROM TFOSMT05A"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 30 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" GROUP BY ELM_DESC"
							" ORDER BY ELM_DESC"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//02改钢情况
				tbl = "TFOSMT05D1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A10'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT05D2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, SHIFT_GROUP, HEAT_NO, OLD_ST_NO, ST_NO, REMARK"
							" FROM TFOSMT05B"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 3 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//03恒拉速率指标（二炼钢）
				tbl = "TFOSMT05E";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH T AS ( SELECT DATE_TIME, SHIFT_GROUP, CC_MACH_NO, PRODUCT_CHARGE, QUALIFIED_CHARGE,"
							" ROUND(QUALIFIED_CHARGE/PRODUCT_CHARGE, 4) QUALIFIED_RATE, REMARK"
							" FROM TFOSMT05C"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME = @DATE_TIME )"
							" SELECT *"
							" FROM T"
							" WHERE SHIFT_GROUP = 'A'"
							" UNION ALL"
							" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM T"
							" WHERE SHIFT_GROUP = 'A'"
							" GROUP BY DATE_TIME, SHIFT_GROUP"
							" "
							" UNION ALL"
							" SELECT *"
							" FROM T"
							" WHERE SHIFT_GROUP = 'B'"
							" UNION ALL"
							" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM T"
							" WHERE SHIFT_GROUP = 'B'"
							" GROUP BY DATE_TIME, SHIFT_GROUP"
							" "
							" UNION ALL"
							" SELECT *"
							" FROM T"
							" WHERE SHIFT_GROUP = 'C'"
							" UNION ALL"
							" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM T"
							" WHERE SHIFT_GROUP = 'C'"
							" GROUP BY DATE_TIME, SHIFT_GROUP"
							" "
							" UNION ALL"
							" SELECT *"
							" FROM T"
							" WHERE SHIFT_GROUP = 'D'"
							" UNION ALL"
							" SELECT DATE_TIME, SHIFT_GROUP, '合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM T"
							" WHERE SHIFT_GROUP = 'D'"
							" GROUP BY DATE_TIME, SHIFT_GROUP"
							" "
							" UNION ALL"
							" SELECT DATE_TIME, '当日合计' SHIFT_GROUP, '当日合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM T"
							" WHERE 1 = 1"
							" GROUP BY DATE_TIME"
							" "
							" UNION ALL"
							" SELECT DATE_TIME, '当日合计' SHIFT_GROUP, '当日合计' CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE),"
							" ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE, ' ' REMARK"
							" FROM TFOSMT05C"
							" WHERE FACTORY_DIV = 'A20'"
							" AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(@DATE_TIME, 1, 6)"
							" GROUP BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//3#CC、4#CC符合率（每日）
				tbl = "TFOSMT05F";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, CC_MACH_NO, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE), ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE"
							" FROM TFOSMT05C"
							" WHERE FACTORY_DIV = 'A20'"
							" AND DATE_TIME > TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAY,'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" GROUP BY DATE_TIME, CC_MACH_NO"
							" ORDER BY DATE_TIME, CC_MACH_NO"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//3#CC、4#CC月度符合率（分班组）
				tbl = "TFOSMT05G";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT SHIFT_GROUP, SUM(PRODUCT_CHARGE), SUM(QUALIFIED_CHARGE), ROUND(SUM(PRODUCT_CHARGE) / SUM(QUALIFIED_CHARGE), 4) QUALIFIED_RATE"
							" FROM TFOSMT05C"
							" WHERE FACTORY_DIV = 'A20'"
							" AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(@DATE_TIME, 1, 6)"
							" GROUP BY SHIFT_GROUP"
							" ORDER BY SHIFT_GROUP"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 6)
			{
				//公司考核指标
				tbl = "TFOSMT06";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH T AS ("
							" SELECT A.FACTORY_DIV, A.DEP_NAME, A.RATE RATE_TARGET,"
							" B1.RATE RATE_1,"
							" B2.RATE RATE_2,"
							" B3.RATE RATE_3,"
							" B4.RATE RATE_4,"
							" B5.RATE RATE_5,"
							" CAST(ROUND((B1.RATE + B2.RATE + B3.RATE + B4.RATE + B5.RATE) / 5, 3) AS DECIMAL(5,3)) RATE_AVG"
							" FROM TFOSMT06B A"
							" LEFT JOIN TFOSMT06A B1 ON A.FACTORY_DIV = B1.FACTORY_DIV AND A.DEP_NAME = B1.DEP_NAME AND B1.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							" LEFT JOIN TFOSMT06A B2 ON A.FACTORY_DIV = B2.FACTORY_DIV AND A.DEP_NAME = B2.DEP_NAME AND B2.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							" LEFT JOIN TFOSMT06A B3 ON A.FACTORY_DIV = B3.FACTORY_DIV AND A.DEP_NAME = B3.DEP_NAME AND B3.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							" LEFT JOIN TFOSMT06A B4 ON A.FACTORY_DIV = B4.FACTORY_DIV AND A.DEP_NAME = B4.DEP_NAME AND B4.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							" LEFT JOIN TFOSMT06A B5 ON A.FACTORY_DIV = B5.FACTORY_DIV AND A.DEP_NAME = B5.DEP_NAME AND B5.DATE_TIME = TO_CHAR(TO_DATE(@DATE_TIME,'YYYYMMDD') - 4 DAY,'YYYYMMDD')"
							" ORDER BY FACTORY_DIV, DEP_NAME"
							" )"
							" SELECT *"
							" FROM T"
							" WHERE FACTORY_DIV = 'A10'"
							" UNION ALL"
							" SELECT FACTORY_DIV, '综合' DEP_NAME, SUM(RATE_TARGET), SUM(RATE_1), SUM(RATE_2), SUM(RATE_3), SUM(RATE_4), SUM(RATE_5), SUM(RATE_AVG)"
							" FROM T"
							" WHERE FACTORY_DIV = 'A10'"
							" GROUP BY FACTORY_DIV"
							" "
							" UNION ALL"
							" SELECT *"
							" FROM T"
							" WHERE FACTORY_DIV = 'A20'"
							" UNION ALL"
							" SELECT FACTORY_DIV, '综合' DEP_NAME, SUM(RATE_TARGET), SUM(RATE_1), SUM(RATE_2), SUM(RATE_3), SUM(RATE_4), SUM(RATE_5), SUM(RATE_AVG)"
							" FROM T"
							" WHERE FACTORY_DIV = 'A20'"
							" GROUP BY FACTORY_DIV"
							" "
							" UNION ALL"
							" SELECT '合计', '', AVG(RATE_TARGET), AVG(RATE_1), AVG(RATE_2), AVG(RATE_3), AVG(RATE_4), AVG(RATE_5), AVG(RATE_AVG)"
							" FROM"
							" ("
							" SELECT FACTORY_DIV, '综合' DEP_NAME, SUM(RATE_TARGET) RATE_TARGET, SUM(RATE_1) RATE_1, SUM(RATE_2) RATE_2, SUM(RATE_3) RATE_3,"
							" SUM(RATE_4) RATE_4, SUM(RATE_5) RATE_5, SUM(RATE_AVG) RATE_AVG"
							" FROM T"
							" WHERE FACTORY_DIV = 'A10'"
							" GROUP BY FACTORY_DIV"
							" UNION ALL"
							" SELECT FACTORY_DIV, '综合' DEP_NAME, SUM(RATE_TARGET), SUM(RATE_1), SUM(RATE_2), SUM(RATE_3), SUM(RATE_4), SUM(RATE_5), SUM(RATE_AVG)"
							" FROM T"
							" WHERE FACTORY_DIV = 'A20'"
							" GROUP BY FACTORY_DIV"
							" ) T2"
							" "
							" ORDER BY FACTORY_DIV, DEP_NAME"
							;
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

			}
			else if (page_id == 7)
			{
				//七、资源综合回收利用
				tbl = "TFOSMT07A";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD')),"
							" C AS ("
							" SELECT MAT_TYPE, MAT_NAME, FACTORY_DIV, SUM(DEVO_WT) DEVO_WT"
							" FROM TFOSMT07A"
							" WHERE 1 = 1 AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(@DATE_TIME, 1, 6)"
							" GROUP BY MAT_TYPE, MAT_NAME, FACTORY_DIV"
							" )"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
							" B1.DEVO_WT DEVO_WT_1, B2.DEVO_WT DEVO_WT_2, B3.DEVO_WT DEVO_WT_3,"
							" B1.DEVO_WT + B2.DEVO_WT+ B3.DEVO_WT DEVO_WT_4,"
							" B4.DEVO_WT DEVO_WT_5, B5.DEVO_WT DEVO_WT_6,"
							" B4.DEVO_WT + B5.DEVO_WT DEVO_WT_7,"
							" B6.DEVO_WT DEVO_WT_8, B7.DEVO_WT DEVO_WT_9,"
							" B6.DEVO_WT + B7.DEVO_WT DEVO_WT_10"
							" FROM A"
							" LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_TYPE = '1' AND B1.MAT_NAME = '渣钢大块'"
							" LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_TYPE = '1' AND B2.MAT_NAME = '渣钢粒'"
							" LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_TYPE = '1' AND B3.MAT_NAME = '豆钢'"
							" LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_TYPE = '2' AND B4.MAT_NAME = '渣铁大块'"
							" LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_TYPE = '2' AND B5.MAT_NAME = '渣铁300'"
							" LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_TYPE = '3' AND B6.FACTORY_DIV = 'A10'"
							" LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_TYPE = '3' AND B7.FACTORY_DIV = 'A20'"
							" UNION ALL"
							" SELECT '合计', C1.DEVO_WT, C2.DEVO_WT, C3.DEVO_WT, C1.DEVO_WT + C2.DEVO_WT + C3.DEVO_WT,"
							" C4.DEVO_WT, C5.DEVO_WT, C4.DEVO_WT + C5.DEVO_WT,"
							" C6.DEVO_WT, C7.DEVO_WT, C6.DEVO_WT + C7.DEVO_WT"
							" FROM SYSIBM.SYSDUMMY1"
							" LEFT JOIN C C1 ON C1.MAT_TYPE = '1' AND C1.MAT_NAME = '渣钢大块'"
							" LEFT JOIN C C2 ON C2.MAT_TYPE = '1' AND C2.MAT_NAME = '渣钢粒'"
							" LEFT JOIN C C3 ON C3.MAT_TYPE = '1' AND C3.MAT_NAME = '豆钢'"
							" LEFT JOIN C C4 ON C4.MAT_TYPE = '2' AND C4.MAT_NAME = '渣铁大块'"
							" LEFT JOIN C C5 ON C5.MAT_TYPE = '2' AND C5.MAT_NAME = '渣铁300'"
							" LEFT JOIN C C6 ON C6.MAT_TYPE = '3' AND C6.FACTORY_DIV = 'A10'"
							" LEFT JOIN C C7 ON C7.MAT_TYPE = '3' AND C7.FACTORY_DIV = 'A20'"
							" WHERE 1 = 1"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT07B";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD')),"
							" C AS ("
							" SELECT MAT_TYPE, MAT_NAME, FACTORY_DIV, SUM(DEVO_WT) DEVO_WT"
							" FROM TFOSMT07A"
							" WHERE 1 = 1 AND SUBSTR(DATE_TIME, 1, 6) = SUBSTR(@DATE_TIME, 1, 6)"
							" GROUP BY MAT_TYPE, MAT_NAME, FACTORY_DIV"
							" )"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME,"
							" B1.DEVO_WT DEVO_WT_1, B2.DEVO_WT DEVO_WT_2, B3.DEVO_WT DEVO_WT_3,"
							" B1.DEVO_WT + B2.DEVO_WT+ B3.DEVO_WT DEVO_WT_4,"
							" B4.DEVO_WT DEVO_WT_5, B5.DEVO_WT DEVO_WT_6, B6.DEVO_WT DEVO_WT_7,"
							" B4.DEVO_WT + B5.DEVO_WT + B6.DEVO_WT DEVO_WT_8,"
							" B7.DEVO_WT DEVO_WT_9, B8.DEVO_WT DEVO_WT_10,"
							" B7.DEVO_WT + B8.DEVO_WT DEVO_WT_11,"
							" B9.DEVO_WT DEVO_WT_12, BA.DEVO_WT DEVO_WT_13,"
							" B9.DEVO_WT + BA.DEVO_WT DEVO_WT_14"
							" FROM A"
							" LEFT JOIN TFOSMT07A B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.MAT_TYPE = '4' AND B1.MAT_NAME = '渣钢'"
							" LEFT JOIN TFOSMT07A B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.MAT_TYPE = '4' AND B2.MAT_NAME = '切割'"
							" LEFT JOIN TFOSMT07A B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.MAT_TYPE = '4' AND B3.MAT_NAME = '中包'"
							" LEFT JOIN TFOSMT07A B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.MAT_TYPE = '5' AND B4.MAT_NAME = '渣盆'"
							" LEFT JOIN TFOSMT07A B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.MAT_TYPE = '5' AND B5.MAT_NAME = '落锤'"
							" LEFT JOIN TFOSMT07A B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.MAT_TYPE = '5' AND B6.MAT_NAME = '中包'"
							" LEFT JOIN TFOSMT07A B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.MAT_TYPE = '6' AND B7.FACTORY_DIV = 'A10'"
							" LEFT JOIN TFOSMT07A B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.MAT_TYPE = '6' AND B8.FACTORY_DIV = 'A20'"
							" LEFT JOIN TFOSMT07A B9 ON TO_CHAR(A.TIME,'YYYYMMDD') = B9.DATE_TIME AND B9.MAT_TYPE = '7' AND B9.FACTORY_DIV = 'A10'"
							" LEFT JOIN TFOSMT07A BA ON TO_CHAR(A.TIME,'YYYYMMDD') = BA.DATE_TIME AND BA.MAT_TYPE = '7' AND BA.FACTORY_DIV = 'A20'"
							" UNION ALL"
							" SELECT '合计', C1.DEVO_WT, C2.DEVO_WT, C3.DEVO_WT, C1.DEVO_WT + C2.DEVO_WT + C3.DEVO_WT,"
							" C4.DEVO_WT, C5.DEVO_WT, C6.DEVO_WT, C4.DEVO_WT + C5.DEVO_WT + C6.DEVO_WT,"
							" C7.DEVO_WT, C8.DEVO_WT, C7.DEVO_WT + C8.DEVO_WT,"
							" C9.DEVO_WT, CA.DEVO_WT, C9.DEVO_WT + CA.DEVO_WT"
							" FROM SYSIBM.SYSDUMMY1"
							" LEFT JOIN C C1 ON C1.MAT_TYPE = '4' AND C1.MAT_NAME = '渣钢'"
							" LEFT JOIN C C2 ON C2.MAT_TYPE = '4' AND C2.MAT_NAME = '切割'"
							" LEFT JOIN C C3 ON C3.MAT_TYPE = '4' AND C3.MAT_NAME = '中包'"
							" LEFT JOIN C C4 ON C4.MAT_TYPE = '5' AND C4.MAT_NAME = '渣盆'"
							" LEFT JOIN C C5 ON C5.MAT_TYPE = '5' AND C5.MAT_NAME = '落锤'"
							" LEFT JOIN C C6 ON C6.MAT_TYPE = '5' AND C6.MAT_NAME = '中包'"
							" LEFT JOIN C C7 ON C7.MAT_TYPE = '6' AND C7.FACTORY_DIV = 'A10'"
							" LEFT JOIN C C8 ON C8.MAT_TYPE = '6' AND C8.FACTORY_DIV = 'A20'"
							" LEFT JOIN C C9 ON C9.MAT_TYPE = '7' AND C9.FACTORY_DIV = 'A10'"
							" LEFT JOIN C CA ON CA.MAT_TYPE = '7' AND CA.FACTORY_DIV = 'A20'"
							" WHERE 1 = 1"
							" ORDER BY DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 8)
			{
				//八、KPI指标
				tbl = "TFOSMT08";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A1.SEQ_NO, A1.ITEM_CNAME, A1.UNIT,"
							" A1.INDEX_TARGET INDEX_TARGET_1, B1.INDEX_DAY INDEX_DAY_1, B1.INDEX_TOL INDEX_TOL_1,"
							" A2.INDEX_TARGET INDEX_TARGET_2, B2.INDEX_DAY INDEX_DAY_2, B2.INDEX_TOL INDEX_TOL_2"
							" FROM TFOSMT08B A1"
							" LEFT JOIN TFOSMT08B A2 ON A1.SEQ_NO = A2.SEQ_NO AND A2.FACTORY_DIV = 'A20'"
							" LEFT JOIN TFOSMT08A B1 ON A1.SEQ_NO = B1.SEQ_NO AND B1.FACTORY_DIV = 'A10' AND B1.DATE_TIME = @DATE_TIME"
							" LEFT JOIN TFOSMT08A B2 ON A1.SEQ_NO = B2.SEQ_NO AND B2.FACTORY_DIV = 'A20' AND B2.DATE_TIME = @DATE_TIME"
							" WHERE A1.FACTORY_DIV = 'A10'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 9)
			{
				tbl = "TFOSMT09";
				{
					//九、主要能源介质实绩
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A1.ENERGY_CODE, A1.ENERGY_CNAME, A1.UNIT, A1.PRICE, A1.VALUE_REAL, A1.VALUE_TARGET,"
							" B1.VALUE_DAY VALUE_DAY_1, B2.VALUE_DAY VALUE_DAY_2, B3.VALUE_DAY VALUE_DAY_3, B4.VALUE_DAY VALUE_MONTH"
							" FROM TFOSMT09B A1"
							" LEFT JOIN TFOSMT09A B1 ON A1.ENERGY_CODE = B1.ENERGY_CODE AND B1.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 2 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT09A B2 ON A1.ENERGY_CODE = B2.ENERGY_CODE AND B2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 1 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT09A B3 ON A1.ENERGY_CODE = B3.ENERGY_CODE AND B3.DATE_TIME = @DATE_TIME"
							" LEFT JOIN (SELECT ENERGY_CODE, SUM(VALUE_DAY) VALUE_DAY FROM TFOSMT09A WHERE SUBSTR(DATE_TIME,1,6) = SUBSTR(@DATE_TIME,1,6) GROUP BY ENERGY_CODE) B4"
							" ON A1.ENERGY_CODE = B4.ENERGY_CODE"
							" WHERE 1 = 1"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 10)
			{
				//十、专项指标
				//倒罐脱硫
				tbl = "TFOSMT10A";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME,"
							" A.IRON_S IRON_S_1, A.IRON_TEMP IRON_TEMP_1, A.MOLTIRON_WT MOLTIRON_WT_1,"
							" A.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_1, A.RATE RATE_1,"
							" B.IRON_S IRON_S_2, B.IRON_TEMP IRON_TEMP_2, B.MOLTIRON_WT MOLTIRON_WT_2,"
							" B.SCRAPE_SLAG_WGT SCRAPE_SLAG_WGT_2, B.RATE RATE_2"
							" FROM TFOSMT10A A"
							" LEFT JOIN TFOSMT10A B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
							" WHERE A.DATE_TIME <= @DATE_TIME AND A.DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							" AND A.FACTORY_DIV = 'A10'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_avg = { "IRON_S_1", "IRON_S_2", "IRON_TEMP_1", "IRON_TEMP_2",
												"MOLTIRON_WT_1", "MOLTIRON_WT_2", "SCRAPE_SLAG_WGT_1", "SCRAPE_SLAG_WGT_2",
												"RATE_1", "RATE_2" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = row[col_avg[j]].ToDecimal() / count;
					}
				}

				//高炉
				tbl = "TFOSMT10B1";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.IRON_S VALUE"
							" FROM TFOSMT10I A"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" AND STATION_NO = '2'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT10B2";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.IRON_S VALUE"
							" FROM TFOSMT10I A"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" AND STATION_NO = '4'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
				tbl = "TFOSMT10B3";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.IRON_S VALUE"
							" FROM TFOSMT10I A"
							" WHERE 1 = 1"
							" AND DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 7 DAYS),'YYYYMMDD')"
							" AND DATE_TIME <= @DATE_TIME"
							" AND STATION_NO = '5'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}

				//转炉精炼鱼雷罐
				tbl = "TFOSMT10C";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.RAW_IRON_WT RAW_IRON_WT_1, A.SCRAP_STEEL_IN SCRAP_STEEL_IN_1,"
							" A.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_1, A.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_1,"
							" A.RATE RATE_1, A.SCRAP_STEEL_SR SCRAP_STEEL_SR_1,"
							" B.RAW_IRON_WT RAW_IRON_WT_1, B.SCRAP_STEEL_IN SCRAP_STEEL_IN_1,"
							" B.SCRAP_STEEL_BOF SCRAP_STEEL_BOF_2, B.SCRAP_STEEL_HEAT SCRAP_STEEL_HEAT_2,"
							" B.RATE RATE_2, B.SCRAP_STEEL_SR SCRAP_STEEL_SR_2, B.SCRAP_STEEL_TPD,"
							" A.SCRAP_STEEL_TPC + B.SCRAP_STEEL_TPC SCRAP_STEEL_TPC, (A.RATIO_NUM + B.RATIO_NUM) / 2 RATIO_NUM"
							" FROM TFOSMT10B A"
							" LEFT JOIN TFOSMT10B B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND A.DATE_TIME <= @DATE_TIME AND A.DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "RAW_IRON_WT_1", "RAW_IRON_WT_2", "SCRAP_STEEL_IN_1", "SCRAP_STEEL_IN_2",
												"SCRAP_STEEL_BOF_1", "SCRAP_STEEL_BOF_2", "SCRAP_STEEL_HEAT_1", "SCRAP_STEEL_HEAT_2",
												"SCRAP_STEEL_SR_1", "SCRAP_STEEL_SR_2", "SCRAP_STEEL_TPD" };
					vector<CString> col_avg = { "RATE_1", "RATE_2", "RATIO_NUM" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = row[col_avg[j]].ToDecimal() / count;
					}
				}

				//硫指标
				tbl = "TFOSMT10D";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.PRODUCT_CHARGE PRODUCT_CHARGE_1, A.QUALIFIED_CHARGE QUALIFIED_CHARGE_1,"
							" A.QUALIFIED_CHARGE / A.PRODUCT_CHARGE QUALIFIED_RATE_1, A.ACT_COSTING ACT_COSTING_1,"
							" B.PRODUCT_CHARGE PRODUCT_CHARGE_2, B.QUALIFIED_CHARGE QUALIFIED_CHARGE_2,"
							" B.QUALIFIED_CHARGE / B.PRODUCT_CHARGE QUALIFIED_RATE_2, B.ACT_COSTING ACT_COSTING_2,"
							" A.ACT_COSTING + B.ACT_COSTING ACT_COSTING_TOTAL"
							" FROM TFOSMT10C A"
							" LEFT JOIN TFOSMT10C B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND A.DATE_TIME <= @DATE_TIME AND A.DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "PRODUCT_CHARGE_1", "PRODUCT_CHARGE_2", "QUALIFIED_CHARGE_1", "QUALIFIED_CHARGE_2",
												"ACT_COSTING_1", "ACT_COSTING_2", "ACT_COSTING_TOTAL" };
					vector<CString> col_avg = { "QUALIFIED_RATE_1", "QUALIFIED_RATE_2" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = row[col_avg[j]].ToDecimal() / count;
					}
				}

				//铁水装入异常信息
				tbl = "TFOSMT10E";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.DATE_TIME, A.ADJUST_CHARGE ADJUST_CHARGE_1, A.MOLTIRON_WT MOLTIRON_WT_1, A.REMARK REMARK_1,"
							" B.ADJUST_CHARGE ADJUST_CHARGE_2, B.MOLTIRON_WT MOLTIRON_WT_2, B.REMARK REMARK_2, A.ADJUST_CHARGE + B.ADJUST_CHARGE MOLTIRON_WT_TOTAL"
							" FROM TFOSMT10D A"
							" LEFT JOIN TFOSMT10D B ON A.DATE_TIME = B.DATE_TIME AND B.FACTORY_DIV = 'A20'"
							" WHERE A.FACTORY_DIV = 'A10'"
							" AND A.DATE_TIME > TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') - 10 DAYS),'YYYYMMDD')"
							" AND A.DATE_TIME <= @DATE_TIME"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "ADJUST_CHARGE_1", "ADJUST_CHARGE_2", "MOLTIRON_WT_1", "MOLTIRON_WT_2",
												"MOLTIRON_WT_TOTAL" };
					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
					}
				}

				//发热剂使用情况
				tbl = "TFOSMT10F";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 9 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD'))"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.RATE RATE_1, B2.RATE RATE_2, B3.RATE RATE_3, B4.RATE RATE_4,"
							" B5.RATE RATE_5, B6.RATE RATE_6, B7.RATE RATE_7, B8.RATE RATE_8,"
							" B1.RATE + B5.RATE RATE_9, B2.RATE + B6.RATE RATE_10, B3.RATE + B7.RATE RATE_11, B4.RATE + B8.RATE RATE_12"
							" FROM A"
							" LEFT JOIN TFOSMT10E B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.MAT_NAME = '硅铁单耗'"
							" LEFT JOIN TFOSMT10E B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.MAT_NAME = '石墨单耗'"
							" LEFT JOIN TFOSMT10E B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A10' AND B3.MAT_NAME = '发热球单耗'"
							" LEFT JOIN TFOSMT10E B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A10' AND B4.MAT_NAME = '折合石墨单耗'"
							" LEFT JOIN TFOSMT10E B5 ON TO_CHAR(A.TIME,'YYYYMMDD') = B5.DATE_TIME AND B5.FACTORY_DIV = 'A20' AND B5.MAT_NAME = '硅铁单耗'"
							" LEFT JOIN TFOSMT10E B6 ON TO_CHAR(A.TIME,'YYYYMMDD') = B6.DATE_TIME AND B6.FACTORY_DIV = 'A20' AND B6.MAT_NAME = '石墨单耗'"
							" LEFT JOIN TFOSMT10E B7 ON TO_CHAR(A.TIME,'YYYYMMDD') = B7.DATE_TIME AND B7.FACTORY_DIV = 'A20' AND B7.MAT_NAME = '发热球单耗'"
							" LEFT JOIN TFOSMT10E B8 ON TO_CHAR(A.TIME,'YYYYMMDD') = B8.DATE_TIME AND B8.FACTORY_DIV = 'A20' AND B8.MAT_NAME = '折合石墨单耗'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_avg = { "RATE_1", "RATE_2", "RATE_3", "RATE_4",
												"RATE_5", "RATE_6", "RATE_7", "RATE_8",
												"RATE_9", "RATE_10", "RATE_11", "RATE_12" };
					for (int i = count - 1; i >= 0; i--)
					{
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count);
					}
				}

				//精炼生产情况
				tbl = "TFOSMT10G";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" WITH A(LEVEL, TIME) AS"
							" (SELECT 1, TO_DATE(@DATE_TIME,'YYYYMMDD') - 9 DAY"
							" FROM SYSIBM.SYSDUMMY1"
							" WHERE 1 = 1"
							" UNION ALL"
							" SELECT LEVEL + 1,TIME + 1 DAY"
							" FROM SYSIBM.SYSDUMMY1, A"
							" WHERE A.TIME < TO_DATE(@DATE_TIME,'YYYYMMDD'))"
							" SELECT TO_CHAR(A.TIME,'YYYYMMDD') DATE_TIME, B1.PRODUCT_CHARGE + B2.PRODUCT_CHARGE PRODUCT_CHARGE_1,"
							" B1.SMELT_CHARGE SMELT_CHARGE_L1, B1.RATE RATE_L1, B1.TOTAL_DURATION TOTAL_DURATION_L1,"
							" B2.SMELT_CHARGE SMELT_CHARGE_R1, B2.RATE RATE_R1,"
							" B3.PRODUCT_CHARGE + B4.PRODUCT_CHARGE PRODUCT_CHARGE_2,"
							" B3.SMELT_CHARGE SMELT_CHARGE_L2, B3.RATE RATE_L2, B3.TOTAL_DURATION TOTAL_DURATION_L2,"
							" B4.SMELT_CHARGE SMELT_CHARGE_R2, B4.RATE RATE_R2"
							" FROM A"
							" LEFT JOIN TFOSMT10F B1 ON TO_CHAR(A.TIME,'YYYYMMDD') = B1.DATE_TIME AND B1.FACTORY_DIV = 'A10' AND B1.STATION_ID = 'L'"
							" LEFT JOIN TFOSMT10F B2 ON TO_CHAR(A.TIME,'YYYYMMDD') = B2.DATE_TIME AND B2.FACTORY_DIV = 'A10' AND B2.STATION_ID = 'L'"
							" LEFT JOIN TFOSMT10F B3 ON TO_CHAR(A.TIME,'YYYYMMDD') = B3.DATE_TIME AND B3.FACTORY_DIV = 'A20' AND B3.STATION_ID = 'R'"
							" LEFT JOIN TFOSMT10F B4 ON TO_CHAR(A.TIME,'YYYYMMDD') = B4.DATE_TIME AND B4.FACTORY_DIV = 'A20' AND B4.STATION_ID = 'R'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "PRODUCT_CHARGE_1", "PRODUCT_CHARGE_2", "SMELT_CHARGE_L1", "SMELT_CHARGE_L2",
												"SMELT_CHARGE_R1", "SMELT_CHARGE_R2" };
					vector<CString> col_avg = { "RATE_L1", "RATE_L2", "TOTAL_DURATION_L1", "TOTAL_DURATION_L1",
												"RATE_R1", "RATE_R2"};

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count);
					}
				}

				//铁坯比
				tbl = "TFOSMT10H";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT DATE_TIME, SLAB_WT, MOLTIRON_WT, SCRAP_STEEL_TPC, MAT_WT, STOCK_TOTAL_WT,"
							" IRON_STEEL_RATE1, IRON_STEEL_RATE2, DAYS, RATE_FINISH_DAY"
							" FROM TFOSMT10G"
							" WHERE 1 = 1"
							" AND A.DATE_TIME <= @DATE_TIME AND A.DATE_TIME >= SUBSTR(@DATE_TIME, 1, 6) || '01'"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();

					//处理合计行
					CDataRow& row = bcls_ret->Tables[tbl].Rows.Add();
					row["DATE_TIME"] = "月度合计";
					int count = bcls_ret->Tables[tbl].Rows.get_Count() - 1;
					vector<CString> col_sum = { "SLAB_WT", "MOLTIRON_WT", "SCRAP_STEEL_TPC", "STOCK_TOTAL_WT",
												"STOCK_TOTAL_WT" };
					vector<CString> col_avg = { "IRON_STEEL_RATE1", "IRON_STEEL_RATE2" };
					vector<CString> col_last = { "DAYS", "RATE_FINISH_DAY" };

					for (int i = count - 1; i >= 0; i--)
					{
						//求和
						for (int j = 0; j < col_sum.size(); j++)
						{
							row[col_sum[j]] = ((i == count - 1) ? 0 : row[col_sum[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_sum[j]].ToDecimal();
						}
						//求平均
						for (int j = 0; j < col_avg.size(); j++)
						{
							row[col_avg[j]] = ((i == count - 1) ? 0 : row[col_avg[j]].ToDecimal()) + bcls_ret->Tables[tbl].Rows[i][col_avg[j]].ToDecimal();
						}
						//取最后
						if (i == count - 1)
						{
							for (int j = 0; j < col_last.size(); j++)
							{
								row[col_last[j]] = bcls_ret->Tables[tbl].Rows[i][col_last[j]].ToDecimal();
							}
						}

						//只保留近三天数据
						if (i <= count - 4) bcls_ret->Tables[tbl].Rows[i].Delete();
					}

					for (int j = 0; j < col_avg.size(); j++)
					{
						if (count > 0) row[col_avg[j]] = (row[col_avg[j]].ToDecimal() / count);
					}

					//年累计
				}
			}
			else if (page_id == 11)
			{
				//检修计划
				tbl = "TFOMST11";
				{
					bcls_ret->Tables.Add(tbl);
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr =
							" SELECT A.CODE, A.CODE_DESC_2_CONTENT, 'A10' FACTORY_DIV, A.CODE_DESC_1_CONTENT STATION_NAME,"
							" B1.STATION_NO STATION_NO_1, B2.STATION_NO STATION_NO_2, B3.STATION_NO STATION_NO_3, B4.STATION_NO STATION_NO_4,"
							" B5.STATION_NO STATION_NO_5, B6.STATION_NO STATION_NO_6, B7.STATION_NO STATION_NO_7,"
							" B1.START_TIME || '-' || B1.END_TIME PERIOD_TIME, B1.REMARK"
							" FROM TEP0002 A"
							" LEFT JOIN TFOSMT11A B1 ON A.CODE = B1.STATION_ID AND B1.FACTORY_DIV = 'A10' AND B1.DATE_TIME = @DATE_TIME"
							" LEFT JOIN TFOSMT11A B2 ON A.CODE = B2.STATION_ID AND B2.FACTORY_DIV = 'A10' AND B2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 1 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B3 ON A.CODE = B3.STATION_ID AND B3.FACTORY_DIV = 'A10' AND B3.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 2 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B4 ON A.CODE = B4.STATION_ID AND B4.FACTORY_DIV = 'A10' AND B4.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 3 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B5 ON A.CODE = B5.STATION_ID AND B5.FACTORY_DIV = 'A10' AND B5.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 4 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B6 ON A.CODE = B6.STATION_ID AND B6.FACTORY_DIV = 'A10' AND B6.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 5 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B7 ON A.CODE = B7.STATION_ID AND B7.FACTORY_DIV = 'A10' AND B7.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 6 DAYS),'YYYYMMDD')"
							" WHERE A.CODE_CLASS = 'FOSMT2'"
							" UNION ALL"
							" SELECT A.CODE, A.CODE_DESC_2_CONTENT, 'A20' FACTORY_DIV, A.CODE_DESC_1_CONTENT STATION_NAME,"
							" B1.STATION_NO STATION_NO_1, B2.STATION_NO STATION_NO_2, B3.STATION_NO STATION_NO_3, B4.STATION_NO STATION_NO_4,"
							" B5.STATION_NO STATION_NO_5, B6.STATION_NO STATION_NO_6, B7.STATION_NO STATION_NO_7,"
							" B1.START_TIME || '-' || B1.END_TIME PERIOD_TIME, B1.REMARK"
							" FROM TEP0002 A"
							" LEFT JOIN TFOSMT11A B1 ON A.CODE = B1.STATION_ID AND B1.FACTORY_DIV = 'A20' AND B1.DATE_TIME = @DATE_TIME"
							" LEFT JOIN TFOSMT11A B2 ON A.CODE = B2.STATION_ID AND B2.FACTORY_DIV = 'A20' AND B2.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 1 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B3 ON A.CODE = B3.STATION_ID AND B3.FACTORY_DIV = 'A20' AND B3.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 2 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B4 ON A.CODE = B4.STATION_ID AND B4.FACTORY_DIV = 'A20' AND B4.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 3 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B5 ON A.CODE = B5.STATION_ID AND B5.FACTORY_DIV = 'A20' AND B5.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 4 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B6 ON A.CODE = B6.STATION_ID AND B6.FACTORY_DIV = 'A20' AND B6.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 5 DAYS),'YYYYMMDD')"
							" LEFT JOIN TFOSMT11A B7 ON A.CODE = B7.STATION_ID AND B7.FACTORY_DIV = 'A20' AND B7.DATE_TIME = TO_CHAR((TO_DATE(@DATE_TIME,'YYYYMMDD') + 6 DAYS),'YYYYMMDD')"
							" WHERE A.CODE_CLASS = 'FOSMT2'"
							" ORDER BY FACTORY_DIV, CODE_DESC_2_CONTENT"
							;
						break;
					}
					cmd_inq.Parameters.Set("DATE_TIME", date_time);
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteQuery(bcls_ret->Tables[tbl]);
					cmd_inq.Close();
				}
			}
			else if (page_id == 12)
			{

			}
		}

		//返回提示栏信息
		CString ts = ((CDecimal)(CDateTime::Now() - datetime).TotalMilliseconds()).Round(0).ToString();
		CFormattable arguments[] = { ts };
		CMessageFormat::Format(s.msg, "数据读取成功！SVC用时[{0}ms]", arguments, 1);
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
