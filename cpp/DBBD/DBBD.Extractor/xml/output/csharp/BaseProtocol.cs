using DBBD;
using System;
using System.Linq;
using System.Text;
using System.Collections.Generic;

class ServerConnectReq : DBBD.Request
{
	public ServerConnectReq()
	{
		typeId = (uint)ProtocolType.Value.ServerConnectReq;
		fingerPrinter.AddRange(Enumerable.Repeat(false, 1));
	}

	public override void Serialize(DBBD.Buffer buffer)
	{
		base.WriteHeader(buffer, GetLength());
		DBBD.Serialize.Write(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD.Serialize.Write(buffer, sessionId); }
	}

	public void Deserialize(DBBD.Buffer buffer)
	{
		base.ReadHeader(buffer);
		DBBD.Deserialize.Read(buffer, out fingerPrinter);
		if (fingerPrinter[0]) { DBBD.Deserialize.Read(buffer, out sessionId); }
	}

	public uint GetLength()
	{
		uint totalLength = 0;
		totalLength += (uint)(base.GetLength() + sizeof(uint) + fingerPrinter.Count);
		if (fingerPrinter[0]) { totalLength += (uint)(sizeof(long)); }
		return totalLength;
	}

	public long SessionId { get { return sessionId; } set { sessionId = value; fingerPrinter[0] = true; } }

	private List<bool> fingerPrinter = new List<bool>();
	private long sessionId;
}

class ServerConnectResp : DBBD.Response
{
	public ServerConnectResp()
	{
		typeId = (uint)ProtocolType.Value.ServerConnectResp;
	}

	public override void Serialize(DBBD.Buffer buffer)
	{
		base.WriteHeader(buffer, GetLength());
	}

	public void Deserialize(DBBD.Buffer buffer)
	{
		base.ReadHeader(buffer);
	}

	public uint GetLength()
	{
		uint totalLength = 0;
		return totalLength;
	}
}

// 핑 요청
}

class PingCheckReq : DBBD.Request
{
	public PingCheckReq()
	{
		typeId = (uint)ProtocolType.Value.PingCheckReq;
	}

	public override void Serialize(DBBD.Buffer buffer)
	{
		base.WriteHeader(buffer, GetLength());
	}

	public void Deserialize(DBBD.Buffer buffer)
	{
		base.ReadHeader(buffer);
	}

	public uint GetLength()
	{
		uint totalLength = 0;
		return totalLength;
	}
}

// 핑 응답
}

class PingCheckResp : DBBD.Response
{
	public PingCheckResp()
	{
		typeId = (uint)ProtocolType.Value.PingCheckResp;
	}

	public override void Serialize(DBBD.Buffer buffer)
	{
		base.WriteHeader(buffer, GetLength());
	}

	public void Deserialize(DBBD.Buffer buffer)
	{
		base.ReadHeader(buffer);
	}

	public uint GetLength()
	{
		uint totalLength = 0;
		return totalLength;
	}
}

