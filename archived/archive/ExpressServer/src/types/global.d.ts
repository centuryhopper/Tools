import { JwtPayload } from "jsonwebtoken";
import { IAccountService } from "../services/IAccountService";
import { IPlaidItemService } from "../services/IPlaidItemService";

declare global {
  namespace Express {
    interface Request {
      payload?: JwtPayload;
      accountService?: IAccountService;
      plaidItemService?: IPlaidItemService;
    }
  }
}
