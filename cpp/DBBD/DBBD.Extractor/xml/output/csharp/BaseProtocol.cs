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

class ServerConnectResp : DBBD.Response
{
	public ServerConnectResp()
	{
		typeId = (uint)ProtocolType.Value.ServerConnectResp;
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
		if (fingerPrinter[0]) { totalLength += (uint)(sizeof(int)); }
		return totalLength;
	}

	public int SessionId { get { return sessionId; } set { sessionId = value; fingerPrinter[0] = true; } }

	private List<bool> fingerPrinter = new List<bool>();
	private int sessionId;
}

class RelayNoti : DBBD.Request
{
	public RelayNoti()
	{
		typeId = (uint)ProtocolType.Value.RelayNoti;
		fingerPrinter.AddRange(Enumerable.Repeat(false, 10));
	}

	public override void Serialize(DBBD.Buffer buffer)
	{
		base.WriteHeader(buffer, GetLength());
		DBBD.Serialize.Write(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD.Serialize.Write(buffer, userId); }
		if (fingerPrinter[1]) { DBBD.Serialize.Write(buffer, relayType); }
		if (fingerPrinter[2]) { DBBD.Serialize.Write(buffer, intValue01); }
		if (fingerPrinter[3]) { DBBD.Serialize.Write(buffer, intValue02); }
		if (fingerPrinter[4]) { DBBD.Serialize.Write(buffer, intValue03); }
		if (fingerPrinter[5]) { DBBD.Serialize.Write(buffer, intValue04); }
		if (fingerPrinter[6]) { DBBD.Serialize.Write(buffer, floatValue01); }
		if (fingerPrinter[7]) { DBBD.Serialize.Write(buffer, floatValue02); }
		if (fingerPrinter[8]) { DBBD.Serialize.Write(buffer, floatValue03); }
		if (fingerPrinter[9]) { DBBD.Serialize.Write(buffer, floatValue04); }
	}

	public void Deserialize(DBBD.Buffer buffer)
	{
		base.ReadHeader(buffer);
		DBBD.Deserialize.Read(buffer, out fingerPrinter);
		if (fingerPrinter[0]) { DBBD.Deserialize.Read(buffer, out userId); }
		if (fingerPrinter[1]) { DBBD.Deserialize.Read(buffer, out relayType); }
		if (fingerPrinter[2]) { DBBD.Deserialize.Read(buffer, out intValue01); }
		if (fingerPrinter[3]) { DBBD.Deserialize.Read(buffer, out intValue02); }
		if (fingerPrinter[4]) { DBBD.Deserialize.Read(buffer, out intValue03); }
		if (fingerPrinter[5]) { DBBD.Deserialize.Read(buffer, out intValue04); }
		if (fingerPrinter[6]) { DBBD.Deserialize.Read(buffer, out floatValue01); }
		if (fingerPrinter[7]) { DBBD.Deserialize.Read(buffer, out floatValue02); }
		if (fingerPrinter[8]) { DBBD.Deserialize.Read(buffer, out floatValue03); }
		if (fingerPrinter[9]) { DBBD.Deserialize.Read(buffer, out floatValue04); }
	}

	public uint GetLength()
	{
		uint totalLength = 0;
		totalLength += (uint)(base.GetLength() + sizeof(uint) + fingerPrinter.Count);
		if (fingerPrinter[0]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[1]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[2]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[3]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[4]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[5]) { totalLength += (uint)(sizeof(int)); }
		if (fingerPrinter[6]) { totalLength += (uint)(floatValue01.GetLength()); }
		if (fingerPrinter[7]) { totalLength += (uint)(floatValue02.GetLength()); }
		if (fingerPrinter[8]) { totalLength += (uint)(floatValue03.GetLength()); }
		if (fingerPrinter[9]) { totalLength += (uint)(floatValue04.GetLength()); }
		return totalLength;
	}

	public int UserId { get { return userId; } set { userId = value; fingerPrinter[0] = true; } }
	public int RelayType { get { return relayType; } set { relayType = value; fingerPrinter[1] = true; } }
	public int IntValue01 { get { return intValue01; } set { intValue01 = value; fingerPrinter[2] = true; } }
	public int IntValue02 { get { return intValue02; } set { intValue02 = value; fingerPrinter[3] = true; } }
	public int IntValue03 { get { return intValue03; } set { intValue03 = value; fingerPrinter[4] = true; } }
	public int IntValue04 { get { return intValue04; } set { intValue04 = value; fingerPrinter[5] = true; } }
	public float FloatValue01 { get { return floatValue01; } set { floatValue01 = value; fingerPrinter[6] = true; } }
	public float FloatValue02 { get { return floatValue02; } set { floatValue02 = value; fingerPrinter[7] = true; } }
	public float FloatValue03 { get { return floatValue03; } set { floatValue03 = value; fingerPrinter[8] = true; } }
	public float FloatValue04 { get { return floatValue04; } set { floatValue04 = value; fingerPrinter[9] = true; } }

	private List<bool> fingerPrinter = new List<bool>();
	private int userId;
	private int relayType;
	private int intValue01;
	private int intValue02;
	private int intValue03;
	private int intValue04;
	private float floatValue01;
	private float floatValue02;
	private float floatValue03;
	private float floatValue04;
}

