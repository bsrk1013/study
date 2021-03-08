// import ServerErrorCode from "../ServerErrorCode";
// import SnackbarUtil from "../components/SnackbarUtil";
// import { Logout } from "../components/UserLoginout";

export interface Response<T> {
  success: boolean;
  data: T;
  resultCode: number;
}

export function responseReturn<T>(result: Response<T>) {
  switch (
    result.resultCode
    // case ServerErrorCode.NO_DATA:
    //   SnackbarUtil.warning("데이터가 없습니다.");
    //   break;
    // case ServerErrorCode.USER_NOT_FOUND:
    //   SnackbarUtil.warning("계정 정보가 잘못되었습니다.");
    //   Logout();
    //   break;
  ) {
  }
  return result;
}
