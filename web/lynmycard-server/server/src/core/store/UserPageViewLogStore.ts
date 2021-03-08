import { UserPageViewLogRepository } from "../repository/UserPageViewLogRepository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import { patch } from "../util/Patch";

namespace UserPageViewLogStore {
  export async function getLogs(startDate: Date, endDate: Date) {
    return await using(new MySqlConn(getLogs.name), async (mysql) => {
      return await getLogsNoneUsing(mysql, startDate, endDate);
    });
  }

  export async function getLogsNoneUsing(
    mysql: MySqlConn,
    startDate: Date,
    endDate: Date,
  ) {
    const entities = await new UserPageViewLogRepository(mysql).selectLogs(
      startDate,
      endDate,
    );

    return entities.map((x) => x.getData());
  }
}

patch({ UserPageViewLogStore });

export default UserPageViewLogStore;
