using DBBD;
using System;
using System.Linq;
using System.Text;
using System.Collections.Generic;

 static class ProtocolType
{
	public enum Value
	{
		PingCheckReq = 1001,
		PingCheckResp = 1002,
	}
	private static Dictionary<Value, string> stringMap = new Dictionary<Value, string>
	{
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
