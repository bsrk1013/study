import axios, { AxiosResponse } from "axios";
import { PayLetterConfig, ServerConfig } from "../../Config";
import { createHash } from "crypto";
import Log from "./Log";

namespace PayLetter {
  export interface CustomParameter {
    userId: number;
    userName: string;
    pid: number;
    expireDate: Date;
    autopayFlag: boolean;
    host: string;
  }

  export interface Receipt {
    amount: number;
    billkey: string;
    card_info: string;
    cid: string;
    custom_parameter: string;
    discount_amount: number;
    domestic_flag: string;
    install_month: string;
    nonsettle_amount: number;
    order_no: string;
    pay_info: string;
    payhash: string;
    pgcode: string;
    product_name: string;
    service_name: string;
    tax_amount: number;
    taxfree_amount: number;
    tid: string;
    transaction_date: string;
    user_id: string;
    user_name: string;
  }

  export interface AutopayResult {
    tid: string;
    cid: string;
    amount: number;
    billkey: string;
    transaction_date: Date;
  }

  // 결제 요청
  export async function Request(
    host: string,
    pgcode: string,
    userId: number,
    userName: string,
    serviceName: string,
    pid: number,
    amount: number,
    productName: string,
    emailAddr: string,
    returnUrl: string,
    cancelUrl: string,
    callbackUrl: string,
    customParameter: CustomParameter,
    emailFlag: boolean = true,
    autopayFlag: boolean = false,
    receiptFlag: boolean = true,
  ): Promise<AxiosResponse<any> | void> {
    const data = {
      pgcode: pgcode,
      user_id: userId,
      user_name: userName,
      service_name: serviceName,
      client_id: PayLetterConfig.ClientId,
      order_no: pid,
      amount: amount,
      product_name: productName,
      email_flag: emailFlag ? "Y" : "N",
      email_addr: emailAddr,
      autopay_flag: autopayFlag ? "Y" : "N",
      receipt_flag: receiptFlag ? "Y" : "N",
      custom_parameter: JSON.stringify(customParameter),
      return_url: `${returnUrl}`,
      cancel_url: `${cancelUrl}`,
      callback_url: `${callbackUrl}`,
    };

    console.log(data);

    const result = await axios
      .post(`${PayLetterConfig.BaseUrl}/v1.0/payments/request`, data, {
        headers: { Authorization: `PLKEY ${PayLetterConfig.ApiBillingKey}` },
      })
      .catch((error) => {
        Log.error(error);
      });

    return result;
  }

  // 결제 취소
  export async function Cancel(
    pgcode: string,
    userId: number,
    tid: string,
    amount: number,
  ) {
    const result = await axios.post(
      `${PayLetterConfig.BaseUrl}/v1.0/payments/cancel`,
      {
        pgcode: pgcode,
        user_id: userId,
        client_id: PayLetterConfig.ClientId,
        tid: tid,
        amount: amount,
        ip_addr: ServerConfig.Host,
      },
      {
        headers: { Authorization: `PLKEY ${PayLetterConfig.ApiBillingKey}` },
      },
    );
    return result;
  }

  export function VerifyReceipt(
    payHash: string,
    userId: string,
    amount: number,
    tid: string,
  ) {
    const rawHash = payHash;
    const newHash = createHash("sha256")
      .update(userId + amount + tid + PayLetterConfig.ApiBillingKey)
      .digest("hex");

    return (
      rawHash.toLocaleLowerCase().localeCompare(newHash.toLocaleLowerCase()) ===
      0
    );
  }

  export async function Autopay(
    serviceName: string,
    userId: number,
    userEmail: string,
    pgcode: string,
    pid: number,
    billingKey: string,
    productName: string,
    price: number,
  ) {
    const data = {
      pgcode: pgcode,
      client_id: PayLetterConfig.ClientId,
      service_name: serviceName,
      user_id: userId,
      user_name: userEmail,
      order_no: pid,
      amount: price,
      product_name: productName,
      billkey: billingKey,
      ip_addr: PayLetterConfig.ApiUrl,
    };

    const result = await axios
      .post(`${PayLetterConfig.BaseUrl}/v1.0/payments/autopay`, data, {
        headers: { Authorization: `PLKEY ${PayLetterConfig.ApiBillingKey}` },
      })
      .catch((error) => {
        Log.error(error);
      });

    return result;
  }
}

export default PayLetter;
