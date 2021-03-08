import { Day, Hour, Minute } from "./core/CoreConst";

export const AuthTokenExpireOffsetMs = 7 * Day;
export const SignUpAuthExpireOffsetMs = 3 * Minute;

export const TemporaryDeviceExpireOffsetMs = 2 * Hour;

export const MaxDeviceCount = 5;

export const NameMinSize = 2;
export const NameMaxSize = 20;

export const PasswordMinSize = 8;
export const PasswordMaxSize = 16;

export enum BookType {
  WebToon,
}

export type BookSortMethod = "none" | "latest_episode";

export const DormancyAccountMonth = 12;
