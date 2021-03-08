import { MySqlConn } from "../db/MySqlDB";
import { using } from "../util/Using";
import { UserLoginTokenRepository, UserLoginTokenSchema } from "../repository";
import Log from "../util/Log";
import { patch } from "../util/Patch";

export class UserLoginTokenDto {
  userId: number;
  deviceId: string;
  authToken: string;
  lastRegDate: Date;

  constructor(entity: any) {
    this.userId = entity.userId;
    this.deviceId = entity.deviceId;
    this.authToken = entity.authToken;
    this.lastRegDate = entity.lastRegDate;
  }

  toDao(mysql: MySqlConn) {
    return new UserLoginTokenRepository(mysql, <UserLoginTokenSchema>{
      userId: this.userId,
      deviceId: this.deviceId,
      authToken: this.authToken,
      lastRegDate: this.lastRegDate,
    });
  }
}

namespace UserLoginTokenStore {
  export async function exists(
    userId: number,
    deviceId: string,
  ): Promise<boolean> {
    return await using(new MySqlConn(exists.name), async (mysql) => {
      const result = await new UserLoginTokenRepository(mysql).findAll({
        where: { userId, deviceId },
      });

      return result.length !== 0;
    });
  }

  export async function findOne(
    userId: number,
    deviceId: string,
  ): Promise<UserLoginTokenDto | null> {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const schema = await new UserLoginTokenRepository(mysql).findOneAllowNull(
        {
          userId,
          deviceId,
        },
      );

      if (schema) {
        return new UserLoginTokenDto(schema);
      } else {
        return null;
      }
    });
  }

  export async function save(
    userId: number,
    deviceId: string,
    authToken: string,
    lastRegDate: string,
  ): Promise<boolean> {
    return await using(new MySqlConn(save.name), async (mysql) => {
      try {
        await new UserLoginTokenRepository(mysql).insertOnDuplicateUpdate(
          userId,
          deviceId,
          authToken,
          lastRegDate,
        );
      } catch (error) {
        Log.error(error);
        return false;
      }
      return true;
    });
  }
}

patch({ UserLoginTokenStore });

export default UserLoginTokenStore;
