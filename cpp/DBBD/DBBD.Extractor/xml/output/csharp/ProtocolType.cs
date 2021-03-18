using DBBD;
using System;
using System.Linq;
using System.Text;
using System.Collections.Generic;

 static class ProtocolType
{
	public enum Value
	{
		ServerConnectReq = 1001,
		ServerConnectResp = 1002,
		RelayNoti = 1003,
	}
	private static Dictionary<Value, string> stringMap = new Dictionary<Value, string>
	{
		{ Value.ServerConnectReq, "ServerConnectReq" },
		{ Value.ServerConnectResp, "ServerConnectResp" },
		{ Value.RelayNoti, "RelayNoti" },
	}
	public static string Get(Value value)
	{
		if (!stringMap.ContainsKey(value))
		{
			return "";
		}
		return stringMap[value];
	}
}
