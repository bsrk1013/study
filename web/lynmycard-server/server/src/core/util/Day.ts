export default class Day {
  public static readonly Days = [0, 1, 2, 3, 4, 5, 6];
  public static readonly DayStrings = [
    "월",
    "화",
    "수",
    "목",
    "금",
    "토",
    "일",
  ];
  value: number;

  constructor(day: 0 | 1 | 2 | 3 | 4 | 5 | 6) {
    this.value = day;
  }

  toString = () => {
    return Day.DayStrings[this.value];
  };
}
