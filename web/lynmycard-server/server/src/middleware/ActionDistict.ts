import Express from "express";

export async function ActionDistictForUser(
  req: Express.Request,
  resp: Express.Response,
  next: Express.NextFunction,
) {
  console.log(req.url);
  console.log(req.baseUrl);
  console.log(req.originalUrl);

  resp.sendStatus(403);
  //   next();
}

export function ActionDistictForIP(req, res, next) {}
