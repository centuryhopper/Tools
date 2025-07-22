import axios from "axios";
import { GeneralResponse, LoginDTO, LoginResponse } from "../types";
import { db } from "../utils/db";
import { generateToken } from "../utils/jwt";
import { IAccountService } from "./IAccountService";

export class AccountService implements IAccountService
{
  async logout(userId: number): Promise<GeneralResponse>
  {
    try
    {
      await db.query(
        `UPDATE users SET datelastlogout = CURRENT_TIMESTAMP WHERE id = $1`,
        [userId]
      );
      return { flag: true, message: "Log out success!" };
    } catch (err: any)
    {
      return { flag: false, message: err.message };
    }
  }

  async loginAccount(loginDTO: LoginDTO): Promise<LoginResponse>
  {
    try
    {
      const response = await axios.post(
        "https://dotnetusermanagementsystem-production.up.railway.app/api/UMS/get-user-credentials?appName=FinanceApp",
        loginDTO
      );
      const { userId, username, email, roles } = response.data;

      const result = await db.query(
        "SELECT * FROM users WHERE ums_userid = $1",
        [userId]
      );

      // console.log("result", result);

      let user: any;
      if (result.rows.length === 0)
      {
        const insertResult = await db.query(
          `INSERT INTO users (ums_userid, email, datecreated, datelastlogin)
           VALUES ($1, $2, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)
           RETURNING *`,
          [userId, email]
        );
        user = insertResult.rows[0];
      } else
      {
        user = result.rows[0];
        await db.query(
          "UPDATE users SET datelastlogin = CURRENT_TIMESTAMP WHERE id = $1",
          [user.id]
        );
      }

      const token = generateToken({
        id: user.id,
        umsUserid: user.ums_userid,
        email: user.email,
        role: roles[0],
      });

      return { flag: true, token, message: "Login completed" };
    } catch (err: any)
    {
      return { flag: false, message: err.message };
    }
  }
}
