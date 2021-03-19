using DBBD;
using System;
using System.Linq;
using System.Text;
using System.Collections.Generic;

 static class Base
{
	public enum Value
	{
		ServerConnectReq = 1001,
		ServerConnectResp = 1002,
		PingCheckReq = 1003,
		PingCheckResp = 1004,
	}
	private static Dictionary<Value, string> stringMap = new Dictionary<Value, string>
	{
		{ Value.ServerConnectReq, "ServerConnectReq" },
		{ Value.ServerConnectResp, "ServerConnectResp" },
		{ Value.PingCheckReq, "PingCheckReq" },
		{ Value.PingCheckResp, "PingCheckResp" },
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
 static class Relay
{
	public enum Value
	{
		RelayNoti = 2001,
	}
	private static Dictionary<Value, string> stringMap = new Dictionary<Value, string>
	{
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
