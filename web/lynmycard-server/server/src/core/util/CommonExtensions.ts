interface Date {
  GetRemainMiliSeconds: () => Number;
}

Date.prototype.GetRemainMiliSeconds = function (): Number {
  const now = new Date();
  const milliseconds = now.getTime() - this.getTime();
  return milliseconds;
};

interface Number {
  GetDays: () => Number;
  GetHours: () => Number;
  GetMinutes: () => Number;
  GetSeconds: () => Number;
}

Number.prototype.GetDays = function (): Number {
  const days = this.GetDays / 24;
  return Math.floor(days);
};

Number.prototype.GetHours = function (): Number {
  const hours = this.GetMinutes() / 60;
  return Math.floor(hours);
};

Number.prototype.GetMinutes = function (): Number {
  const minutes = this.GetSeconds() / 60;
  return Math.floor(minutes);
};

Number.prototype.GetSeconds = function (): Number {
  const seconds = this / 1000;
  return Math.floor(seconds);
};
