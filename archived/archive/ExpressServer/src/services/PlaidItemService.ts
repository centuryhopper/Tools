import { decrypt, encrypt } from "./encryption-context";
import { GeneralResponse, PlaidItemDTO } from "../types";
import { db } from "../utils/db";
import { IPlaidItemService } from "./IPlaidItemService";

export class PlaidItemService implements IPlaidItemService
{
  async getAccessTokensAsync(userId: number): Promise<PlaidItemDTO[]>
  {
    const record = await db.query(
      "SELECT * from plaiditems where userid = $1",
      [userId]
    );

    if (record.rows.length === 0)
    {
      return []
    }

    return record.rows.map(x =>
    {
      const item = x as PlaidItemDTO
      item.accesstoken = decrypt(item.access_token_iv!, item.accesstoken);
      return item
    })
  }
  async editPlaidItemAsync(dto: PlaidItemDTO): Promise<GeneralResponse>
  {
    if (!dto)
    {
      return {
        flag: false,
        message: "dto cannot be null",
      };
    }

    const record = await this.getPlaidItemAsync(dto.userid, dto.institutionname)
    if (!record)
    {
      return {
        flag: false,
        message: `Plaid item doesn't exist for ${dto.institutionname}!`,
      };
    }


    const { iv, encryptedData } = encrypt(record.accesstoken)

    try
    {
      await db.query(
        `UPDATE plaiditems
          SET accesstoken = $1,
          access_token_iv = $2,
          datelinked = $3
        WHERE userid = $4 AND institutionname = $5
        `,
        [encryptedData, iv, new Date(), record.userid, record.institutionname]
      );

      return { flag: true, message: "Plaid item edited" };
    } catch (e)
    {
      return { flag: false, message: e as string };
    }
  }
  async getPlaidItemAsync(
    userId: number,
    institution_name: string
  ): Promise<PlaidItemDTO | undefined>
  {
    const record = await db.query(
      "SELECT * from plaiditems where userid = $1 AND institutionname = $2",
      [userId, institution_name]
    );

    if (record.rows.length === 0)
    {
      return undefined
    }

    const value = record.rows[0] as PlaidItemDTO
    if (value.access_token_iv)
    {
      value.accesstoken = decrypt(value.access_token_iv, value.accesstoken)
    }

    return value;
  }
  async storePlaidItemAsync(dto?: PlaidItemDTO): Promise<GeneralResponse>
  {
    if (!dto)
    {
      return {
        flag: false,
        message: "dto cannot be null",
      };
    }

    const record = await this.getPlaidItemAsync(dto.userid, dto.institutionname);

    if (record)
    {
      return {
        flag: false,
        message: `Plaid item already exists for ${dto.institutionname}!`,
      };
    }

    try
    {

      const { iv, encryptedData } = encrypt(dto.accesstoken)

      await db.query(
        `INSERT INTO plaiditems (userid, accesstoken, institutionname, datelinked, access_token_iv, transactions_cursor)
    VALUES (
        $1, $2, $3, $4, $5, $6
      )
        `,
        [dto.userid, encryptedData, dto.institutionname, dto.datelinked, iv, dto.transactions_cursor]
      );

      return { flag: true, message: "Plaid item added" };
    } catch (e)
    {
      return { flag: false, message: e as string };
    }
  }
}
