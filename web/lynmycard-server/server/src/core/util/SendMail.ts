import * as NodeMailer from "nodemailer";
import AWS from "aws-sdk";
import { AwsConfig, Configs } from "../../Config";
import Log from "./Log";

export async function SendMail(userEmail: string, html: string) {
  // if (Configs.env.localeCompare("development") === 0) {
  //   Log.debug("send mail by google");
  //   SendMailByGoogleSmtp(userEmail, html);
  // } else {
  Log.debug("send mail by aws");
  SendMailByAwsSES(userEmail, html);
  // }
}

export async function SendMailByGoogleSmtp(userEmail: string, html: string) {
  const transporter = NodeMailer.createTransport({
    service: "gmail",
    port: 587,
    host: "smtp.gmail.com",
    secure: false,
    requireTLS: true,
    auth: {
      user: "",
      pass: "",
    },
  });

  const mailOptions = {
    from: "shynitoon@gmail.com",
    to: userEmail,
    subject: "웹툰에듀 인증",
    html: html,
  };

  await transporter.sendMail(mailOptions);
}

export async function SendMailByAwsSES(userEmail: string, html: string) {
  AWS.config.update({
    accessKeyId: AwsConfig.accessKeyId,
    secretAccessKey: AwsConfig.secretAccessKey,
    region: AwsConfig.region,
  });

  const ses = new AWS.SES();

  const params = {
    Destination: {
      ToAddresses: [userEmail], // 받는 사람
      CcAddresses: [], // 참조
      BccAddresses: [], // 숨은 참조
    },
    Message: {
      Body: {
        Html: {
          Data: html,
          Charset: "utf-8",
        },
      },
      Subject: {
        Data: "웹툰에듀 인증",
        Charset: "utf-8",
      },
    },
    Source: AwsConfig.sesSourceMail, // 보낸 사람
    ReplyToAddresses: [], // 답장 받을 이메일
  };

  ses.sendEmail(params, (error, data) => {
    if (error) {
      Log.error(error);
    }
    if (data) {
      Log.debug(data);
    }
  });
}

export function GetFindPasswordMailForm(
  title: string,
  content: string,
  userEmail: string,
  link: string,
) {
  return `
  <link
  rel="stylesheet"
  type="text/css"
  href="https://fonts.googleapis.com/earlyaccess/notosanskr.css"
/>

<div
  style="
    font-family: 'Noto Sans KR', sans-serif;
    display: flex;
    flex-direction: column;
    text-align: center;
    justify-content: center;
  "
>
  <div>
    <img
      src="https://webtoonedu.s3.ap-northeast-2.amazonaws.com/static/image/banner/%EC%86%8D%EB%8B%B4%ED%83%80%EC%9D%B4%ED%8B%80_%EB%A1%A4%EB%A7%81%EB%B0%B0%EB%84%88.jpg"
      alt="TitleImage"
      style="margin: 5px; padding: 5px;"
    />
  </div>
  <div
    style="
      font-family: 'Noto Sans KR', sans-serif;
      display: flex;
      flex-direction: column;
      text-align: center;
      justify-content: center;
    "
  >
    <h1>${title}</h1>
    <br />
    <div>
      <p>안녕하세요. ${userEmail}</p>
      <p>고객님 웹툰에듀입니다.</p>
    </div>
    <br />
    <div>
      <p>${content}</p>
      <p>아래의 링크를 눌러주세요.</p>
      <a
        href="${link}"
        style="
        display: inline-block;
        padding: 15px 80px;
        background-color: white;
        border: solid 1px blue;
        color: blue;
         cursor: pointer;
         font-family: 'Noto Sans KR', sans-serif;
         text-decoration: none;
         margin-top: 15px;">
         이메일 인증
      </a>
    </div>
  </div>
</div>
  `;
}

export function GetAuthMailForm(
  title: string,
  content: string,
  userEmail: string,
  authCode: string,
) {
  return `
  <link
  rel="stylesheet"
  type="text/css"
  href="https://fonts.googleapis.com/earlyaccess/notosanskr.css"
/>

<div
  style="
    font-family: 'Noto Sans KR', sans-serif;
    display: flex;
    flex-direction: column;
    text-align: center;
    justify-content: center;
  "
>
  <div>
    <img
      src="https://webtoonedu.s3.ap-northeast-2.amazonaws.com/static/image/banner/%EC%86%8D%EB%8B%B4%ED%83%80%EC%9D%B4%ED%8B%80_%EB%A1%A4%EB%A7%81%EB%B0%B0%EB%84%88.jpg"
      alt="TitleImage"
      style="margin: 5px; padding: 5px;"
    />
  </div>
  <div
    style="
      font-family: 'Noto Sans KR', sans-serif;
      display: flex;
      flex-direction: column;
      text-align: center;
      justify-content: center;
    "
  >
    <h1>${title}</h1>
    <br />
    <div>
      <p>안녕하세요. ${userEmail}</p>
      <p>고객님 웹툰에듀입니다.</p>
    </div>
    <br />
    <div>
      <p>${content}</p>
      <p>아래의 인증코드를 입력해 주세요.</p>
      <p>${authCode}</p>
    </div>
  </div>
</div>
  `;
}

export function DormancyNotiMailForm() {
  return `
  <link
  rel="stylesheet"
  type="text/css"
  href="https://fonts.googleapis.com/earlyaccess/notosanskr.css"
/>

<div
  style="
    font-family: 'Noto Sans KR', sans-serif;
    display: flex;
    flex-direction: column;
    text-align: center;
    justify-content: center;
  "
>
  <div>
    <img
      src="https://webtoonedu.s3.ap-northeast-2.amazonaws.com/static/image/banner/%EC%86%8D%EB%8B%B4%ED%83%80%EC%9D%B4%ED%8B%80_%EB%A1%A4%EB%A7%81%EB%B0%B0%EB%84%88.jpg"
      alt="TitleImage"
      style="margin: 5px; padding: 5px;"
    />
  </div>
  <div
    style="
      font-family: 'Noto Sans KR', sans-serif;
      display: flex;
      flex-direction: column;
      text-align: center;
      justify-content: center;
    "
  >
    <p>여기에 텍스트</p>
  </div>
</div>
  `;
}
