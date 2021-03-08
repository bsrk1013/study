import Express from "express";
import Hash from "object-hash";
import { nanoid } from "nanoid";
import { SendMail, GetFindPasswordMailForm } from "../core/util/SendMail";
import {
  AuthType,
  LoginType,
  UserSchema,
  AuthSchema,
} from "../core/repository";
import AuthStore, { AuthDto } from "../core/store/AuthStore";
import UserStore, { UserDto } from "../core/store/UserStore";
import UserLoginTokenStore from "../core/store/UserLoginTokenStore";
import { ResponseScope } from "../Response";
import {
  NameMinSize,
  NameMaxSize,
  PasswordMinSize,
  PasswordMaxSize,
  AuthTokenExpireOffsetMs,
} from "../AppConst";
import Time from "../core/util/Time";
import { ServerConfig } from "../Config";
import ErrorCode from "../core/ErrorCode";
import { FavoriteStore } from "../core/store";
import { using } from "../core/util/Using";
import { MySqlConn } from "../core/db/MySqlDB";
import Log from "../core/util/Log";

const router = Express.Router();

type SignUpResult = {
  result: boolean;
  userId: number;
};

function EmailCheck(userEmail: string) {
  if (userEmail.length <= 0) {
    return false;
  }

  {
    const idArray = userEmail.split("@");
    if (idArray.length !== 2) {
      return false;
    }

    const pattern = /[\{\}\[\]\/?,;:|\)*~`!^\<>@\#$%&\\\=\(\'\"]/gi;
    const regex = new RegExp(pattern);
    if (regex.exec(idArray[0])) {
      return false;
    }
  }

  {
    if (userEmail.includes(" ")) {
      return false;
    }
  }

  {
    const idArray = userEmail.split(".");
    if (idArray.length <= 1) {
      return false;
    } else {
      var tempString = idArray[idArray.length - 1];
      if (tempString.length <= 0) {
        return false;
      }
    }
  }
  return true;
}

function NameCheck(name: string) {
  {
    if (name.length < NameMinSize || NameMaxSize < name.length) {
      return false;
    }
  }

  {
    const pattern = /([^가-힣a-zA-Z0-9\x20])/i;
    const regex = new RegExp(pattern);
    if (regex.exec(name)) {
      return false;
    }
  }

  return true;
}

function PasswordCheck(password: string) {
  const pattern = /^[0-9a-zA-Z]+$/;
  const regex = new RegExp(pattern);
  return (
    !regex.exec(password) ||
    password.length < PasswordMinSize ||
    PasswordMaxSize < password.length
  );
}

router.get("/emailcheck", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail: string = req.query.userEmail as string;

    if (!EmailCheck(userEmail)) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    var exists = await UserStore.exists(userEmail);
    if (exists) {
      scope.setResultCode(ErrorCode.USER_EMAIL_DUPLICATED);
      return;
    }

    scope.setResult(true);
  });
});

router.get("/namecheck", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const name: string = req.query.name as string;

    if (!NameCheck(name)) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const existsIllegalNickname = await UserStore.existsIllegalNickname(name);
    if (existsIllegalNickname) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const existsNickname = await UserStore.existsNickname(name);
    if (existsNickname) {
      scope.setResultCode(ErrorCode.USER_NICKNAME_DUPLICATED);
    }

    scope.setResult(true);
  });
});

router.post("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail: string = req.body.userEmail;
    const hashPassword = req.body.password;
    const name = req.body.name;
    const userType = req.body.userType;
    const deviceId = req.body.deviceId;
    const marketingAgree = req.body.marketingAgree;

    const createResult = createUser(
      userEmail,
      hashPassword,
      name,
      userType,
      marketingAgree,
    );
    if (createResult.resultCode !== ErrorCode.OK || !createResult.userInfo) {
      Log.error(
        `${scope.getName()}, user create faield, userEmail:${userEmail}, name:${name}`,
      );
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const result = await registUser(createResult.userInfo, deviceId);

    scope.setData(result);
    scope.setResult(true);
  });

  const createUser = (
    userEmail: string,
    hashPassword: string,
    name: string,
    userType: number,
    marketingAgree: boolean,
  ) => {
    if (!EmailCheck(userEmail)) {
      return { resultCode: ErrorCode.INVALID_PARAMETER };
    }

    if (!NameCheck(name)) {
      return { resultCode: ErrorCode.INVALID_PARAMETER };
    }

    // if(!PasswordCheck(hashPassword)){
    //   return { resultCode: ErrorCode.INVALID_PARAMETER };
    // }

    return {
      resultCode: ErrorCode.OK,
      userInfo: <UserSchema>{
        id: -1,
        userEmail: userEmail,
        password: hashPassword,
        userType: userType,
        name: name,
        lastLoginDate: Time.getNow(),
        lastLogoutDate: Time.getNow(),
        subscriptionStartDate: Time.getDefaultTime(),
        subscriptionExpireDate: Time.getDefaultTime(),
        privacyAgree: marketingAgree ? 1 : 0,
        signUpAuthAgree: 1,
      },
    };
  };

  const registUser = async (
    userData: UserSchema,
    deviceId: string,
  ): Promise<SignUpResult> => {
    const existsUserInfo = await UserStore.exists(userData.userEmail);
    if (existsUserInfo) {
      throw ErrorCode.USER_EMAIL_DUPLICATED;
    }

    const existsNickname = await UserStore.existsNickname(userData.name);
    if (existsNickname) {
      throw ErrorCode.USER_NICKNAME_DUPLICATED;
    }

    const user = await UserStore.insert(userData);

    return <SignUpResult>{
      result: true,
      userId: user.id,
    };
  };
});

router.delete("/", async (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail: string = req.body.userEmail;
    const hashPassword: string = req.query.password as string;

    Log.info(
      `${scope.getName()}, userEmail:${userEmail}, password:${hashPassword}`,
    );

    var user = await UserStore.findOneByPassword(userEmail, hashPassword);
    if (!user) {
      Log.error(
        `${scope.getName()}, user info not found, userEmail:${userEmail}, hashPassword:${hashPassword}`,
      );
      scope.setResultCode(ErrorCode.USER_NOT_FOUND);
      return;
    }

    const userId = user.id;
    // 트랜잭션 작업
    await using(new MySqlConn("UserWithdrawal", true), async (mysql) => {
      var sql = `delete from user where id = ${userId}`;
      await mysql.query(sql);
      sql = `delete from user_device where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from user_autopay where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from auth where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from purchase_cancel_log where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from purchase_log where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from user_login_token where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from user_page_view_log where userId = ${userId}`;
      await mysql.query(sql);
      sql = `delete from view_detail_log where userId = ${userId}`;
      await mysql.query(sql);
    });

    await FavoriteStore.removeBookFavorite(userId);

    scope.setResult(true);
  });
});

type LoginResult = {
  userInfo: UserDto | null;
  userLoginToken: string;
};

router.get("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail: string = req.query.userEmail as string;
    const loginType: number = Number.parseInt(req.query.loginType as string);
    const hashPassword: string = req.query.password as string;
    const deviceId: string = req.query.deviceId as string;

    let result: LoginResult = {
      userInfo: null,
      userLoginToken: "",
    };

    let userInfo: UserDto | null = null;
    if (loginType === LoginType.Default) {
      userInfo = await UserStore.findOneByPassword(userEmail, hashPassword);
    } else if (loginType === LoginType.AccessToken) {
      const userId = Number.parseInt(userEmail);
      const tokenInfo = await UserLoginTokenStore.findOne(userId, deviceId);
      if (
        tokenInfo &&
        tokenInfo.lastRegDate.GetRemainMiliSeconds() < AuthTokenExpireOffsetMs
      ) {
        userInfo = await UserStore.findOneByUserLoginToken(
          userId,
          deviceId,
          hashPassword,
        );
      }
    }

    if (userInfo !== null) {
      userInfo.password = "";
      result.userInfo = userInfo;

      const now = Time.getNow();
      await UserStore.update(
        userInfo.id,
        `lastLoginDate = '${Time.convertJsToMySqlFormat(now)}'`,
      );

      const newAuthToken = nanoid();
      const saveAuthTokenResult = await UserLoginTokenStore.save(
        userInfo.id,
        deviceId,
        newAuthToken,
        Time.convertJsToMySqlFormat(Time.getNow()),
      );
      if (saveAuthTokenResult) {
        result.userLoginToken = newAuthToken;
      }
    } else {
      if (loginType === LoginType.Default) {
        const exists = await UserStore.exists(userEmail);
        if (exists) {
          scope.setResultCode(ErrorCode.USER_PASSWORD_FAILED);
        } else {
          scope.setResultCode(ErrorCode.USER_EMAIL_FAILED);
        }
        return;
      }
    }

    scope.setData(result);
    scope.setResult(true);
  });
});

router.get("/refresh", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId: number = req.body.userId;

    const userInfo = await UserStore.findOne(userId);

    scope.setData(userInfo);
    scope.setResult(true);
  });
});

router.put("/marketingagree", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const checked: boolean = req.body.checked;

    await UserStore.update(userId, `privacyAgree = '${checked ? 1 : 0}'`);

    scope.setResult(true);
  });
});

router.put("/name", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const name: string = req.body.name;

    let verify = NameCheck(name);
    if (!verify) {
      Log.error(
        `${scope.getName()}, name check failed, userId:${userId}, name:${name}`,
      );
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const existsIllegalNickname = await UserStore.existsIllegalNickname(name);
    if (existsIllegalNickname) {
      Log.error(
        `${scope.getName()}, contains illegal name, userId:${userId}, name:${name}`,
      );
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const existsNickname = await UserStore.existsNickname(name);
    if (existsNickname) {
      Log.error(
        `${scope.getName()}, duplicated name, userId:${userId}, name:${name}`,
      );
      scope.setResultCode(ErrorCode.USER_NICKNAME_DUPLICATED);
      return;
    }

    const result = await UserStore.update(userId, `name = '${name}'`);
    scope.setResult(result);
  });
});

router.put("/password", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const userEmail = req.body.userEmail;
    const hashOrigin: string = req.body.origin;
    const hashDest: string = req.body.dest;

    let verify = await checkPassword(userEmail, hashOrigin, hashDest);
    if (!verify) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    await UserStore.update(userId, `password = '${hashDest}'`);

    scope.setResult(true);
  });

  async function checkPassword(
    userEmail: string,
    origin: string,
    dest: string,
  ) {
    if (origin.localeCompare(dest) === 0) {
      return false;
    }
    // if (PasswordCheck(origin)) {
    //   return false;
    // }
    // if (PasswordCheck(dest)) {
    //   return false;
    // }

    // const originHashPassword = Hash(origin);
    const user = await UserStore.findOneByPassword(userEmail, origin);

    if (!user) {
      return false;
    }

    return true;
  }
});

router.put("/auth/password", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail = req.body.userEmail;
    const hashPassword: string = req.body.dest;

    // if (!PasswordCheck(dest)) {
    //   scope.setResultCode(ErrorCode.INVALID_PARAMETER);
    //   return;
    // }

    const userInfo = await UserStore.findOneByUserEmail(userEmail);
    if (!userInfo) {
      scope.setResultCode(ErrorCode.USER_NOT_FOUND);
      return;
    }

    await UserStore.update(userInfo.id, `password = '${hashPassword}'`);

    scope.setResult(true);
  });
});

router.post("/password", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail = req.body.userEmail;

    if (!EmailCheck(userEmail)) {
      scope.setResultCode(ErrorCode.INVALID_PARAMETER);
      return;
    }

    const userInfo = await UserStore.findOneByUserEmail(userEmail);
    if (!userInfo) {
      scope.setResultCode(ErrorCode.USER_NOT_FOUND);
      return;
    }

    const authToekn = nanoid();
    const authType = AuthType.FindPassword;
    const now = new Date();

    const authInfo = new AuthDto(<AuthSchema>{
      userId: userInfo.id,
      authToken: authToekn,
      regDate: now,
    });

    const saveResult = await AuthStore.save(authInfo);
    if (saveResult) {
      SendMail(
        userEmail,
        GetFindPasswordMailForm(
          "비밀번호 변경",
          "비밀번호를 변경하시려면",
          userEmail,
          `${ServerConfig.ClientOrigin}/user/password/${authType}/${authToekn}/${userEmail}`,
        ),
      );
    }

    scope.setResult(true);
  });
});

router.put("/dormancy", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const result = await UserStore.update(userId, `isDormancy = 0`);
    scope.setResult(result);
  });
});

router.post("/logout", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    try {
      const userId = req.body.userId as number;

      await UserStore.updateLogoutTime(userId);
      scope.setResult(true);
    } catch (errorCode) {
      scope.setResult(false);
      scope.setResultCode(errorCode);
    }
  });
});

export default router;
