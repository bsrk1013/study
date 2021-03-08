import { AuthSchema, AuthRepository } from "../repository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import { patch } from "../util/Patch";

export class AuthDto {
  id: number;
  userId: number;
  authToken: string;
  regDate: Date;

  constructor(entity: AuthSchema) {
    this.id = entity.id;
    this.userId = entity.userId;
    this.authToken = entity.authToken;
    this.regDate = entity.regDate;
  }

  toDao(mysql: MySqlConn) {
    return new AuthRepository(mysql, <AuthSchema>{
      id: this.id,
      userId: this.userId,
      authToken: this.authToken,
      regDate: this.regDate,
    });
  }
}

namespace AuthStore {
  export async function getAuthInfoByToken(
    userId: number,
    authToken: string,
  ): Promise<AuthDto | null> {
    return await using(
      new MySqlConn(getAuthInfoByToken.name),
      async (mysql) => {
        const entity = await new AuthRepository(mysql).findAll({
          where: {
            userId,
            authToken,
          },
        });

        if (entity.length === 0) {
          return null;
        }

        return new AuthDto(entity[0].getData());
      },
    );
  }

  export async function exists(userId: number): Promise<boolean> {
    return await using(new MySqlConn(exists.name), async (mysql) => {
      return await new AuthRepository(mysql).exists({
        userId,
      });
    });
  }

  export async function save(authInfo: AuthDto): Promise<boolean> {
    return await using(new MySqlConn(save.name), async (mysql) => {
      const userId = authInfo.userId;
      const authToken = authInfo.authToken;
      const regDate = authInfo.regDate;

      await new AuthRepository(mysql, <AuthSchema>{
        userId,
        authToken,
        regDate,
      }).insertOnDuplicate({ authToken, regDate });

      return true;
    });
  }

  export async function remove(userId: number): Promise<boolean> {
    return await using(new MySqlConn(remove.name), async (mysql) => {
      await new AuthRepository(mysql).deleteAll({
        where: {
          userId,
        },
      });

      return true;
    });
  }
}

patch({ AuthStore });

export default AuthStore;
