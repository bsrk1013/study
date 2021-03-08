import {
  PurchaseLogRepository,
  AutopaySchema,
  SubscriptionRepository,
  SubscriptionSchema,
  AutopayRepository,
  PurchaseLogSchema,
} from "../repository";
import { DefaultSearchCount } from "../CoreConst";
import { using } from "../util/Using";
import Time from "../util/Time";
import { MySqlConn } from "../db/MySqlDB";
import { patch } from "../util/Patch";

export class PurchaseLogDto {
  regDate: Date;
  UserId: number;
  purchaseLogType: number;
  name: string;
  isPeriodic: boolean;
  pid: string;
  price: number;

  constructor(entity: PurchaseLogSchema) {
    this.regDate = entity.regDate;
    this.UserId = entity.userId;
    this.purchaseLogType = entity.purchaseLogType;
    this.name = entity.name;
    this.isPeriodic = entity.isPeriodic;
    this.pid = entity.pid;
    this.price = entity.price;
  }
}

export class AutopayDto {
  id: number;
  userId: number;
  userEmail: string;
  regDate: Date;
  nextBillingDate: Date;
  pgcode: string;
  pid: number;
  tid: string;
  billingKey: string;

  constructor(entity: AutopaySchema) {
    this.id = entity.id;
    this.userId = entity.userId;
    this.userEmail = entity.userEmail;
    this.regDate = entity.regDate;
    this.nextBillingDate = entity.nextBillingDate;
    this.pgcode = entity.pgcode;
    this.pid = entity.pid;
    this.tid = entity.tid;
    this.billingKey = entity.billingKey;
  }
}

export class SubscriptionDto {
  id: number;
  name: string;
  isPeriodic: boolean;
  price: number;
  discountPrice: number;
  rewardType: number;
  rewardValue: number;
  isSubscription: number;
  description: string;

  constructor(entity: SubscriptionSchema) {
    this.id = entity.id;
    this.name = entity.name;
    this.isPeriodic = entity.isPeriodic;
    this.price = entity.price;
    this.discountPrice = entity.discountPrice;
    this.rewardType = entity.rewardType;
    this.rewardValue = entity.rewardValue;
    this.isSubscription = entity.isSubscription;
    this.description = entity.description;
  }
}

namespace ProductStore {
  export async function findBySubscription() {
    return await using(
      new MySqlConn(findBySubscription.name),
      async (mysql) => {
        const rows = await new SubscriptionRepository(mysql).findAll();

        if (!rows[0]) {
          return null;
        }

        return rows.map((c) => new SubscriptionDto(c.getData()));
      },
    );
  }

  export async function findOneBySubscription(
    id: number,
  ): Promise<SubscriptionDto> {
    return await using(
      new MySqlConn(findOneBySubscription.name),
      async (mysql) => {
        const rows = await new SubscriptionRepository(mysql).findOne({
          id: id,
        });

        return new SubscriptionDto(rows.getData());
      },
    );
  }

  export async function findAllAutopay() {
    return await using(new MySqlConn(findAllAutopay.name), async (mysql) => {
      let currentCount = 0;
      let searchCount = 1000;
      let autopayList: AutopayDto[] = [];

      while (true) {
        const rows = await new AutopayRepository(mysql).findAll({
          limit: searchCount,
          offset: currentCount,
        });

        if (!rows[0]) {
          break;
        }

        autopayList = autopayList.concat(
          rows.map((c) => new AutopayDto(c.getData())),
        );

        currentCount += searchCount;
      }

      return autopayList;
    });
  }

  export async function findByPurchaseLog(
    userId: number,
    index: number,
  ): Promise<PurchaseLogDto[]> {
    return await using(new MySqlConn(findByPurchaseLog.name), async (mysql) => {
      const rows = await new PurchaseLogRepository(mysql).findAll({
        where: { userId: userId, isPeriodic: false },
        limit: DefaultSearchCount,
        offset: index * DefaultSearchCount,
        orderBy: { sort: "desc", column: "regDate" },
      });

      if (!rows[0]) {
        return [];
      }

      return rows.map((c) => new PurchaseLogDto(c.getData()));
    });
  }

  export async function findOnePurchaseLogByTid(userId: number, tid: string) {
    return await using(
      new MySqlConn(findOnePurchaseLogByTid.name),
      async (mysql) => {
        const rows = await new PurchaseLogRepository(mysql).findOne({
          where: { userId, tid },
        });

        return new PurchaseLogDto(rows.getData());
      },
    );
  }

  export async function findOneAutopay(userId: number) {
    return await using(new MySqlConn(findOneAutopay.name), async (mysql) => {
      const schema = await new AutopayRepository(mysql).findOneAllowNull({
        userId,
      });

      if (schema) {
        return new AutopayDto(schema);
      } else {
        return null;
      }
    });
  }

  export async function saveAutopay(
    userId: number,
    nextBillingDate: string,
    pid: number,
    autopayBillingKey: string,
  ) {
    return await using(new MySqlConn(saveAutopay.name), async (mysql) => {
      await mysql.query(
        `insert into user_autopay(regDate, nextBillingDate, userId, pid, billingKey)` +
          `values('${Time.convertJsToMySqlFormat(
            Time.getNow(),
          )}', '${nextBillingDate}', ${userId}, ${pid}, '${autopayBillingKey}')` +
          `on duplicate key update nextBillingDate = '${nextBillingDate}', billingKey = '${autopayBillingKey}'`,
      );
    });
  }

  export async function savePurchaseLog(
    userId: number,
    purchaseLogType: number,
    name: string,
    isPeriodic: boolean,
    pid: number,
    price: number,
    taxAmount: number,
    cid: string,
    tid: string,
    pgcode: string,
    payInfo: string,
  ) {
    return await using(new MySqlConn(savePurchaseLog.name), async (mysql) => {
      await mysql.query(
        `insert into purchase_log(userId, regDate, purchaseLogType, name, isPeriodic, pid, price, taxAmount, cid, tid, pgcode, payInfo)` +
          ` values(${userId}, '${Time.convertJsToMySqlFormat(
            Time.getNow(),
          )}', ${purchaseLogType}, '${name}', ${
            isPeriodic ? 1 : 0
          }, ${pid}, ${price}, ${taxAmount}, '${cid}', '${tid}', '${pgcode}', '${payInfo}')`,
      );
    });
  }

  export async function updatePurchaseLog(userId: number, tid: string) {
    return await using(new MySqlConn(updatePurchaseLog.name), async (mysql) => {
      await mysql.query(
        `update purchase_log set cancelDate = '${Time.convertJsToMySqlFormat(
          Time.getNow(),
        )}' where userId = ${userId} and tid = '${tid}'`,
      );
    });
  }

  export async function updateAutopay(
    userId: number,
    nextBillingDate: Date,
    tid: string,
    billingKey: string,
  ) {
    return await using(new MySqlConn(updateAutopay.name), async (mysql) => {
      await mysql.query(
        `update user_autopay set nextBillingDate = '${nextBillingDate}', tid = '${tid}', billingKey = '${billingKey}' where userId = ${userId}`,
      );
    });
  }

  export async function deleteAutopay(userId: number) {
    return await using(new MySqlConn(deleteAutopay.name), async (mysql) => {
      await new AutopayRepository(mysql).deleteAll({ where: { userId } });
    });
  }

  export async function savePurchaseCancelLog(
    userId: number,
    cid: string,
    tid: string,
    name: string,
    pid: number,
    price: number,
    pgcode: string,
  ) {
    return await using(
      new MySqlConn(savePurchaseCancelLog.name),
      async (mysql) => {
        await mysql.query(
          `insert into purchase_cancel_log(regDate, userId, cid, tid, name, pid, price, pgcode)` +
            ` values('${Time.convertJsToMySqlFormat(
              Time.getNow(),
            )}', ${userId}, '${cid}', '${tid}', '${name}', ${pid}, ${price}, '${pgcode}')`,
        );
      },
    );
  }

  export async function countByPurchaseLog(userId: number) {
    return await using(
      new MySqlConn(countByPurchaseLog.name),
      async (mysql) => {
        const rows = await mysql.query(
          `select count(*) as 'count' from purchase_log where userId = ${userId} and isPeriodic = 0`,
        );
        return rows[0][0].count;
      },
    );
  }
}

patch({ ProductStore });

export default ProductStore;
