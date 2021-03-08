import Express from "express";
import * as global from "../core/db/MySqlDB";
import { Configs } from "../Config";
import { ResponseScope } from "../Response";
import { using } from "../core/util/Using";

const router = Express.Router();

router.get("/checkAccount", async (req, resp, next) => {
  const userId: string = req.query.userId as string;

  console.log(userId);

  try {
    const result = await using(
      new global.MySqlConn("checkAccount"),
      async (mysql) => {
        // FIXME 린 디비에 맞게 수정 해야함
        const query = `SELECT EXISTS(SELECT * FROM Test.TestTable WHERE name = '${userId}') AS 'exists'`;
        const result = mysql.anyQuery(query);
        return result;
      },
    );

    resp.send({ result: result[0][0].exists });
  } catch (e) {
    console.log(e);
    resp.send({ result: 0 });
  }
});

router.get("/3dot1", async (req, resp, next) => {
  var accountId = req.query.userId as string;
  var facServiceId = `NUS`;
  var facGameId = `Vestria`;
  var facGameName = `Vestria`;
  var facTradeSeq = ``;
  var tradeType = `2`;
  var customerId = ``;
  var itemCode = ``;
  var productName = ``;
  var amount = ``;
  var currency = ``;
  var facKey = `47EA4EBED9D9E88D40A0BF20BE6976A`;
  // const FacServiceId = `NUS`;
  // const FacKey = `47EA4EBED9D9E88D40A0BF20BE6976A`;
  // const TradeType = `2`;

  const result1 = await using(
    new global.MySqlConn("3dot1_1"),
    async (mysql) => {
      const query = `select userdb_id from db_map where user_id = '${accountId}'`;
      const result = mysql.anyQuery(query);
      return result;
    },
  );

  console.log(result1[0][0]);

  const result = await using(new global.MySqlConn("3dot1"), async (mysql) => {
    // FIXME 린 디비에 맞게 수정 해야함
    // const query = `call `
    // const query = `SELECT EXISTS(SELECT * FROM Test.TestTable WHERE name = '${userId}') AS 'exists'`;
    // const result = mysql.anyQuery(query);
    return result;
  });

  const url =
    Configs.env.localeCompare("dev") === 0
      ? "http://testb2b.mycard520.com.tw/MyBillingPay/v1.2/AuthGlobal"
      : "https://b2b.mycard520.com.tw/MyBillingPay/v1.2/AuthGlobal";
});

export default router;
