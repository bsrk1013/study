import "../util/CommonExtensions";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import Time from "../util/Time";
import { UserRepository, UserSchema } from "../repository/UserRepository";
import { UserLoginTokenRepository } from "../repository/UserLoginTokenRepository";
import ErrorCode from "../ErrorCode";
import { AuthTokenExpireOffsetMs } from "../../AppConst";
import { patch } from "../util/Patch";
import { MySqlDao } from "../db/MySqlDao";

export class UserDto {
  id: number;
  userEmail: string;
  password: string;
  userType: number;
  name: string;
  lastLoginDate: Date;
  lastLogoutDate: Date;
  subscriptionStartDate: Date;
  subscriptionExpireDate: Date;
  subscriptionPid: number;
  privacyAgree: boolean;
  signUpAuthAgree: boolean;
  isDormancy: boolean;

  constructor(entity: UserSchema) {
    this.id = entity.id;
    this.userEmail = entity.userEmail;
    this.password = entity.password;
    this.userType = entity.userType;
    this.name = entity.name;
    this.lastLoginDate = entity.lastLoginDate;
    this.lastLogoutDate = entity.lastLogoutDate;
    this.subscriptionStartDate = entity.subscriptionStartDate;
    this.subscriptionExpireDate = entity.subscriptionExpireDate;
    this.subscriptionPid = entity.subscriptionPid;
    this.privacyAgree = entity.privacyAgree ? true : false;
    this.signUpAuthAgree = entity.signUpAuthAgree ? true : false;
    this.isDormancy = entity.isDormancy ? true : false;
  }

  toDao(mysql: MySqlConn) {
    return new UserRepository(mysql, <UserSchema>{
      id: this.id,
      userEmail: this.userEmail,
      password: this.password,
      userType: this.userType,
      name: this.name,
      lastLoginDate: this.lastLoginDate,
      lastLogoutDate: this.lastLogoutDate,
      subscriptionStartDate: this.subscriptionStartDate,
      subscriptionExpireDate: this.subscriptionExpireDate,
      subscriptionPid: this.subscriptionPid,
      privacyAgree: this.privacyAgree ? 1 : 0,
      signUpAuthAgree: this.signUpAuthAgree ? 1 : 0,
      isDormancy: this.isDormancy ? 1 : 0,
    });
  }

  isSubscription() {
    const now = Time.getTimeStamp();
    const expire = Time.convertToMsTsFromDate(
      new Date(this.subscriptionExpireDate)
    );

    return expire > now;
  }
}

namespace UserStore {
  export async function findOne(userId: number) {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const rows = await new UserRepository(mysql).findAll({
        where: {
          id: userId,
        },
      });

      if (rows.length === 0) {
        return null;
      }

      return new UserDto(rows[0].getData());
    });
  }

  export async function findOneByUserEmail(userEmail: string) {
    return await using(
      new MySqlConn(findOneByUserEmail.name),
      async (mysql) => {
        const rows = await new UserRepository(mysql).findAll({
          where: { userEmail },
        });

        return new UserDto(rows[0].getData());
      }
    );
  }

  export async function exists(userEmail: string) {
    return await using(new MySqlConn(exists.name), async (mysql) => {
      const exists = await new UserRepository(mysql).exists({ userEmail });

      return exists;
    });
  }

  export async function existsIllegalNickname(name: string) {
    return await using(
      new MySqlConn(existsIllegalNickname.name),
      async (mysql) => {
        const rows = await mysql.query(
          `select * from tb_illegal_name where illegalName = '${name}'`
        );

        return rows[0][0];
      }
    );
  }

  export async function existsNickname(name: string) {
    return await using(new MySqlConn(existsNickname.name), async (mysql) => {
      const exists = await new UserRepository(mysql).exists({
        name,
      });

      return exists;
    });
  }

  export async function findOneByPassword(
    userEmail: string,
    password: string
  ): Promise<null | UserDto> {
    return await using(new MySqlConn(findOneByPassword.name), async (mysql) => {
      const rows = await new UserRepository(mysql).findAll({
        where: {
          userEmail,
          password,
        },
      });

      if (!rows || rows.length === 0) {
        return null;
      }

      return new UserDto(rows[0].getData());
    });
  }

  export async function findOneByUserLoginToken(
    userId: number,
    deviceId: string,
    userLoginToken: string
  ): Promise<UserDto | null> {
    return await using(
      new MySqlConn(findOneByUserLoginToken.name),
      async (mysql) => {
        const tokenRows = await new UserLoginTokenRepository(mysql).findAll({
          where: {
            userId,
            deviceId,
          },
        });

        if (!tokenRows || tokenRows.length === 0) {
          return null;
        }

        const userLoginTokenInfo = {
          authToken: tokenRows[0].getData().authToken,
          lastRegDate: tokenRows[0].getData().lastRegDate,
        };

        const remain = userLoginTokenInfo.lastRegDate.GetRemainMiliSeconds();
        if (remain <= AuthTokenExpireOffsetMs) {
          if (
            userLoginTokenInfo.authToken.localeCompare(userLoginToken) === 0
          ) {
            const user = await new UserRepository(mysql).findOne({
              id: userId,
            });
            return new UserDto(user.getData());
          }
        }
        return null;
      }
    );
  }

  export async function insert(user: UserSchema) {
    return await using(new MySqlConn(insert.name), async (mysql) => {
      const userEntitiy = await new UserRepository(mysql, user).insert();
      return new UserDto(userEntitiy.getData());
    });
  }

  export async function update(userId: number, query: string) {
    return await using(new MySqlConn(update.name), async (mysql) => {
      await mysql
        .query(`update user set ${query} where id = ${userId}`)
        .catch((error) => {
          console.log(error);
          return false;
        });
      return true;
    });
  }

  export async function findAllDormancyUser(targetDate: Date) {
    return await using(
      new MySqlConn(findAllDormancyUser.name),
      async (mysql) => {
        let currentCount = 0;
        let searchCount = 1000;
        let dormancyUserList: UserDto[] = [];

        while (true) {
          const query = `select * from user where lastLoginDate <= '${Time.convertJsToMySqlFormat(
            targetDate
          )}' and isDormancy = 0 limit ${currentCount}, ${searchCount}`;
          const rows = await mysql.query<UserSchema>(query);

          if (!rows[0][0]) {
            break;
          }

          const allocator = new UserRepository(mysql).getAllocator();
          let array: MySqlDao<UserSchema>[] = [];
          for (let i = 0; i < rows[0].length; i++) {
            const data = rows[0][i];
            array.push(allocator(data));
          }

          dormancyUserList = dormancyUserList.concat(
            array.map((c) => new UserDto(c.getData()))
          );

          currentCount += searchCount;
        }

        return dormancyUserList;
      }
    );
  }

  export async function updateDormancyUser(targetId: string) {
    return await using(
      new MySqlConn(updateDormancyUser.name),
      async (mysql) => {
        const query = `update user set isDormancy = 1 where id in(${targetId})`;
        mysql.anyQuery(query);
      }
    );
  }

  export async function updateLogoutTime(userId: number) {
    return await using(new MySqlConn(updateLogoutTime.name), async (mysql) => {
      const user = new UserRepository(mysql);
      if (!(await user.findById(userId))) throw ErrorCode.USER_NOT_FOUND;

      user.getData().lastLogoutDate = Time.getNow();
      await user.update();

      return true;
    });
  }
}

patch({ UserStore });

export default UserStore;
