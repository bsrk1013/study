import Express from "express";
import { SendMail, GetAuthMailForm } from "../core/util/SendMail";
import { AuthType, AuthResultType } from "../core/repository";
import { UserStore } from "../core/store";
import Time from "../core/util/Time";
import { UserDto } from "../core/store/UserStore";
import AuthManager from "../core/util/TimeUpdater/AuthManager";
import { ResponseScope } from "../Response";
import { using } from "../core/util/Using";

const router = Express.Router();

router.post("/user", async (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userEmail: string = req.body.userEmail;
    const deviceId: string = req.body.deviceId;
    const authType = req.body.authType;
    const authToken = Math.floor(Math.random() * 999999) + 100000;

    const result = AuthManager.Instance().insertAuth(
      userEmail,
      deviceId,
      authToken,
      authType,
    );

    if (result) {
      let title = "";
      let content = "";

      switch (authType) {
        case AuthType.SignUp:
          title = "웹툰에듀에 회원가입해주셔서 감사합니다.";
          content = "회원가입을 완료하시려면";
          break;
        case AuthType.RemoveDevice:
          title = "기기 삭제";
          content = "기기 삭제를 완료하시려면";
          break;
        case AuthType.TemporaryDevice:
          title = "임시 기기 등록";
          content = "임시 기기 등록을 완료하시려면";
          break;
        case AuthType.FindPassword:
          title = "비밀번호를 변경";
          content = "비밀번호를 변경하시려면";
          break;
      }

      SendMail(
        userEmail,
        GetAuthMailForm(title, content, userEmail, authToken.toString()),
      );
    }

    const now = Time.getNow();
    scope.setData({ regDate: now });
    scope.setResult(true);
  });
});

router.get("/user", async (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const deviceId: string = req.query.deviceId as string;
    const userEmail: string = req.query.userEmail as string;
    const authType: number = Number.parseInt(req.query.authType as string);
    const authToken: number = Number.parseInt(req.query.authToken as string);

    let result = AuthResultType.Success;

    let userInfo: UserDto | null = null;
    // 회원가입 할때는 유저정보가 없음
    if (authType != AuthType.SignUp) {
      userInfo = await UserStore.findOneByUserEmail(userEmail);

      if (!userInfo) {
        result = AuthResultType.NoUser;
      }
    }

    if (result === AuthResultType.Success) {
      if (
        !AuthManager.Instance().verifyAuth(
          userEmail,
          deviceId,
          authToken,
          authType,
        )
      ) {
        result = AuthResultType.NoAuth;
      }
    }

    scope.setData({ result });
    scope.setResult(true);
  });
});

async function verifyAuthToken(
  userInfo: UserDto | null,
  deviceId: string,
  authType: number,
  authToken: string,
) {
  // 계정정보 없음
  if (!userInfo) {
    return AuthResultType.NoUser;
  }

  if (authType === AuthType.SignUp && userInfo.signUpAuthAgree) {
    return AuthResultType.Success;
  }

  // const authInfo = await AuthStore.getAuthInfoByToken(userInfo.id, authToken);
  // // 인증정보 없음
  // if (authInfo === null) {
  //   return AuthResultType.NoAuth;
  // }

  // const regDate: Date = authInfo.regDate;
  // const remain: Number = regDate.GetRemainMiliSeconds();

  // // 인증시간 만료됨
  // if (remain >= SignUpAuthExpireOffsetMs) {
  //   return AuthResultType.ExpireTime;
  // }

  return AuthResultType.Success;
}

export default router;
