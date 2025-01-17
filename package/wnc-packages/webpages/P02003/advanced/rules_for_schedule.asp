<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

var nat_enable=<%getFwNatEnable();%>;
if(nat_enable!="1")
{
	window.location.href="../login.asp";
}

function schedule_list_action(action)
{
	if(!CheckLoginInfo())
		return false;
	var schedule_list_arr="";
	var schedule_name="";
	document.getElementById('statusMsg').innerHTML="";	
	document.getElementById('ScheduleName').value="";	 
	document.getElementById('ScheduleDelList').value="";
	var editor_action = document.getElementById('ScheduleTblAction');
	var secObj = document.getElementById('tblSchedules');
	var objArr = secObj.getElementsByTagName("INPUT");
	var count=0;
	for (var i=0; i < objArr.length; i++)
	{
		if (objArr[i].type == 'checkbox' && !objArr[i].disabled && objArr[i].name!="imgSelectAllChk")
		{
			if (objArr[i].checked==true)
			{				
				count++;
				var id="schedule_name_"+i;
				schedule_name=document.getElementById(id).innerHTML;
				schedule_list_arr+=schedule_name+",";

				if (checkFieldExist("tblParentCtrlList","profile_schedule_name_",schedule_name,action,gettext("Schedule Name"),gettext("Parental Control Service")))			
					return false;				
			}
		}
	}
	var ret=true;
	if (action=="add")
	{
		editor_action.value = "ScheduleTblAdd";			
	}
	else if (action=="edit")
	{
		if (count > 1)
		{
			alert(gettext("Please select a row to edit."));
			ret=false;
		}
		else if (count==0)
		{
			alert(gettext("Please select a row from the list to be edited."));
			ret=false;
		}	
		else		
		{
			editor_action.value = "ScheduleTblEdit";			
			document.getElementById('ScheduleName').value=schedule_name;					
		}
	}
	else if (action=="delete")
	{
		if (count==0)
		{
			alert(gettext("Please select items from the list to be deleted."));
			ret=false;
		}
		else
		{
			editor_action.value = "ScheduleTblDel";		
			schedule_list_arr=schedule_list_arr.substr(0,schedule_list_arr.length-1);
			document.getElementById('ScheduleDelList').value=schedule_list_arr;			
		}
	}
	return ret;
}

function chkParCtrl()
{
  if (document.getElementById("chkParCtrlEnable").checked==true)
  {
  	document.getElementById("chkParCtrlEnable").value = "on";
  }
  else
  {
  	document.getElementById("chkParCtrlEnable").value = "off";	
  }
}

function pctl_list_action(action)
{
	if(!CheckLoginInfo())
		return false;
	var pctl_list_arr="";
	document.getElementById('PctlProfileName').value="";	 
	document.getElementById('PctlProfileDelList').value="";
	var profile_name;
	var editor_action = document.getElementById('PctlTblAction');
	var secObj = document.getElementById('tblParentCtrlList');
	var objArr = secObj.getElementsByTagName("INPUT");
	var count=0;
	for (var i=0; i < objArr.length; i++)
	{
		if (objArr[i].type == 'checkbox' && !objArr[i].disabled && objArr[i].name!="imgSelectAllChk")
		{
			if (objArr[i].checked==true)
			{
				count++;
				var id="profile_name_"+i;
				profile_name=document.getElementById(id).innerHTML;
				pctl_list_arr+=profile_name+",";
			}
		}
	}
	var ret=true;
	if (action=="add")
	{
		editor_action.value = "PctlTblAdd";			
	}
	else if (action=="edit")
	{
		if (count > 1)
		{
			alert(gettext("Please select a row to edit."));
			ret=false;
		}
		else if (count==0)
		{
			alert(gettext("Please select a row from the list to be edited."));
			ret=false;
		}	
		else		
		{
			editor_action.value = "PctlTblEdit";			
			document.getElementById('PctlProfileName').value=profile_name;								
		}
	}
	else if (action=="delete")
	{
		if (count==0)
		{
			alert(gettext("Please select items from the list to be deleted."));
			ret=false;
		}
		else
		{
			editor_action.value = "PctlTblDel";		
			pctl_list_arr=pctl_list_arr.substr(0,pctl_list_arr.length-1);
			document.getElementById('PctlProfileDelList').value=pctl_list_arr;						
		}
	}
	return ret;
}

function pageLoad()
{
  var ptlCtrlEnable=<%getParCtrl();%>;
  if (ptlCtrlEnable)
  {
  	document.getElementById("chkParCtrlEnable").checked=true;
	document.getElementById("chkParCtrlEnable").value = "on";

	//parental control button className
	document.getElementById("PctlConfAdd").disabled=false;
	document.getElementById("PctlConfEdit").disabled=false;
	document.getElementById("PctlConfDel").disabled=false;	
	document.getElementById("PctlConfAdd").className="tblbtn";
	document.getElementById("PctlConfEdit").className="tblbtn";
	document.getElementById("PctlConfDel").className="tblbtn";
  }
  else
  {
  	document.getElementById("chkParCtrlEnable").checked=false;
	document.getElementById("chkParCtrlEnable").value = "off";	

	document.getElementById("PctlConfAdd").disabled=true;
	document.getElementById("PctlConfEdit").disabled=true;
	document.getElementById("PctlConfDel").disabled=true;
	document.getElementById("PctlConfAdd").className="tblbtn_dis";
	document.getElementById("PctlConfEdit").className="tblbtn_dis";
	document.getElementById("PctlConfDel").className="tblbtn_dis";
  }
  secChkBoxSelectOrUnselectAll('tblParentCtrlList', 'umiId', null);
  secChkBoxSelectOrUnselectAll('tblSchedules', 'umiId', null);
}

function checkApply()
{
	if(!CheckLoginInfo())
		return false;
	return true;
}
</script>
</head>	

<body onload="pageLoad();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("firewall_submenu","firewall_submenu_focus","rules_for_schedule","rules_for_schedule_href");</script>
<!-- Main Menu and Submenu End -->

<!-- Right Content start -->

	<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Rules for Schedule"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg" id="statusMsg"><%getActionResult();%></div>        
        <div class="secH2"><script>document.write(gettext("Parental Control Settings"));</script></div>        
        <div class="secInfo">
			<br><script>document.write(gettext("By configuring some simple parental control profiles, you can protect local network computers from accessing undesirable content or reaching the Internet during scheduled access times."));</script>
			<br>
		</div>
        <form name="frmParentCtlEnable" method="post" action="/goform/setParCtrl?<%getUniName();%>=<%getUniId();%>">
		<input type="hidden" name="parentCtrl_dummy" value="0">  		
			<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
				<tr>
					<td><script>document.write(gettext("Enable Parental Control"));</script></td>
					<td>
						<input type="checkbox" name="chkParCtrlEnable" id="chkParCtrlEnable" onclick="chkParCtrl();">
					<td>&nbsp;</td>
				</tr>
			</table>
			<div class="submitBg">
				<input type="submit" id="pctrl_apply" value="Apply" onclick="return checkApply();" class="submit" title="Apply" name="button.pctlconfig.schedules" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
				<input type="button" id="pctrl_reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
			</div>
		</form>
		
		<div class="secH2"><script>document.write(gettext("List of Parental Control Profiles"));</script></div>
		<form method="post" action="/goform/setUrlProfileTable?<%getUniName();%>=<%getUniId();%>">
			<input type="hidden" id="PctlTblAction" name="PctlTblAction" value="">
 			<input type="hidden" id="PctlProfileName" name="PctlProfileName" value="">
			<input type="hidden" id="PctlProfileDelList" name="PctlProfileDelList" value="">			
			<table border="0" cellpadding="0" cellspacing="0" id="tblParentCtrlList" width="650px" style="table-layout: fixed;" class="specTbl">
				<tr>
					<td class="tdH" width="7%"><input type="checkbox" name="imgSelectAllChk" id="imgSelectAll" onclick="secChkBoxSelectOrUnselectAll ('tblParentCtrlList', 'umiId', this)"></td>
					<td class="tdH" width="25%"><script>document.write(gettext("Profile Name"));</script></td>
					<td class="tdH" width="24%"><script>document.write(gettext("Description"));</script></td>
					<td class="tdH" width="12%"><script>document.write(gettext("Group"));</script></td>
					<td class="tdH" width="20%"><script>document.write(gettext("Session Timeout"));</script></td>
					<td class="tdH" width="12%"><script>document.write(gettext("Inactivity"));</script></td>
				</tr>
				<tbody><%getUrlProfileTable();%></tbody>					
			</table>
			<div id="tblButtons">
				<input type="submit" value="Add" class="tblbtn" name="PctlConfAdd" id="PctlConfAdd" title="Add" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return pctl_list_action('add');">
				<input type="submit" value="Edit" class="tblbtn" name="PctlConfEdit" id="PctlConfEdit" title="Edit" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return pctl_list_action('edit');">
				<input type="submit" value="Delete" class="tblbtn" name="PctlConfDel" id="PctlConfDel" title="Delete" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return pctl_list_action('delete');">
			</div>
		</form>
	
		<div class="secH2"><script>document.write(gettext("List of Schedules"));</script></div>    
	    <div class="secInfo">
			<br><script>document.write(gettext("Schedules are a very useful feature to allow security rules to be enabled or disabled based on the time of day or day of the week. Configured schedules will be available to select in the parental profile configuration page. All schedules will follow the configured system time."));</script>
			<br>
		</div>
		<form method="post" action="/goform/setScheduleTable?<%getUniName();%>=<%getUniId();%>">	
			<input type="hidden" id="ScheduleTblAction" name="ScheduleTblAction" value="">
 			<input type="hidden" id="ScheduleName" name="ScheduleName" value="">
			<input type="hidden" id="ScheduleDelList" name="ScheduleDelList" value="">
			<table border="0" cellpadding="0" cellspacing="0" id="tblSchedules">
			<tr>
				<td class="tdH"><input type="checkbox" name="imgSelectAllChk" id="imgSelectAll" onclick="secChkBoxSelectOrUnselectAll ('tblSchedules', 'umiId', this)"></td>
				<td class="tdH"><script>document.write(gettext("ID"));</script></td>
				<td class="tdH"><script>document.write(gettext("Name"));</script></td>				
			</tr>
			<tbody><%getScheduleTable("schedule");%></tbody>			
		</table>
		<div>
			<input type="submit" class="tblbtn" value="Add" title="Add" id="ScheTblAdd" name="ScheTblAdd" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return schedule_list_action('add');"></td>
			<input type="submit" class="tblbtn" value="Edit" title="Edit" id="ScheTblEdit" name="ScheTblEdit" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return schedule_list_action('edit');"></td>
			<input type="submit" class="tblbtn" value="Delete" title="Delete" id="ScheTblDel" name="ScheTblDel" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return schedule_list_action('delete');"></td>			
		</div>
		</form>
	
	</div>
	<!-- Section End -->
 </div>
 </div>
 <script type="text/javascript">
 	document.getElementById('pctrl_apply').value=gettext("Apply");
	document.getElementById('pctrl_reset').value=gettext("Reset");
	document.getElementById('PctlConfAdd').value=gettext("Add");
	document.getElementById('PctlConfEdit').value=gettext("Edit");
	document.getElementById('PctlConfDel').value=gettext("Delete");
	document.getElementById('ScheTblAdd').value=gettext("Add");
	document.getElementById('ScheTblEdit').value=gettext("Edit");
	document.getElementById('ScheTblDel').value=gettext("Delete");	
</script>
</body>
</html>
