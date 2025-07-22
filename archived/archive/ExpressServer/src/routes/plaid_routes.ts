import { Router } from "express";
import { AuthGetResponse } from "plaid";
import { authenticateToken } from "../middlewares/auth";
import PlaidApiService from "../services/PlaidApiService";
import { logToDb } from "../utils/logger";

const plaidSerivceRouter = () =>
{
  const router = Router();

  const plaidApiService = new PlaidApiService();

  router.get("/test", async (req, res) =>
  {
    await logToDb("info", `Incoming request: ${req.method} ${req.url}`);
    res.status(200).json({
      message: "Logging test completed. Check your PostgreSQL LOGS table.",
    });
  });

  router.get("/get-link-token/:userId", authenticateToken, async (req, res) =>
  {
    const token = await plaidApiService.createLinkToken(req.params.userId);
    res.status(200).json(token);
  });

  // just for sake of testing (WILL NOT USE IN PRODUCTION READY APP)
  router.get("/get-access-tokens/:userId", async (req, res) =>
  {
    const accessTokens = await req.plaidItemService?.getAccessTokensAsync(parseInt(req.params.userId, 10));
    // console.log(accessTokens);
    res.json({ accessTokens })
  })

  // just for sake of testing (WILL NOT USE IN PRODUCTION READY APP)
  router.get("/get-transactions/:accessToken", async (req, res) =>
  {
    const { transactions, cursor } = await plaidApiService.pollTransactions(req.params.accessToken);
    console.log(transactions.length);
    console.log(transactions.map(t => ({ amt: t.amount, merchant_name: t.merchant_name, })));

    res.json(transactions)
  })

  router.post("/exchange-public-token", authenticateToken, async (req, res) =>
  {
    const exchangedResponse = await plaidApiService.exchangePublicToken(
      req.body.public_token
    );

    const userId = parseInt((req.payload?.sub as string) || "0", 10);

    // console.log(userId);

    const authInfo: AuthGetResponse = await plaidApiService.getAuthInfo(
      exchangedResponse.access_token
    );

    const bankInfo = authInfo.accounts.map((acc) => ({
      balances: acc.balances,
      name: acc.name,
      official_name: acc.official_name,
      available: acc.balances.available,
      current: acc.balances.current,
    }));

    const plaidItemDecrypted = await req.plaidItemService?.getPlaidItemAsync(
      userId,
      authInfo.item.institution_name!
    );

    // console.log("transactions", transactions);

    const { transactions, cursor } = await plaidApiService.pollTransactions(
      plaidItemDecrypted?.accesstoken ?? exchangedResponse.access_token
    );

    if (plaidItemDecrypted)
    {
      const { accesstoken, access_token_iv, institutionname, userid, transactions_cursor, } = plaidItemDecrypted

      if (!access_token_iv)
      {
        // update plaiditem field
        const edit_response = await req.plaidItemService?.editPlaidItemAsync({
          access_token_iv: undefined,
          accesstoken,
          datelinked: new Date(),
          institutionname: institutionname,
          transactions_cursor,
          userid,
        })
      }
    }
    else
    {
      const storePlaidItemResponse = await req.plaidItemService?.storePlaidItemAsync({
        userid: userId,
        access_token_iv: undefined,
        transactions_cursor: cursor,
        accesstoken: exchangedResponse.access_token,
        institutionname: authInfo.item.institution_name!,
        datelinked: new Date(),
      });

      // console.log("storePlaidItemResponse: " + JSON.stringify(storePlaidItemResponse))
    }



    res.status(200).json({
      transactions,
      bankInfo,
    });
  });

  return router;
};

const plaidRouter = plaidSerivceRouter()
export default plaidRouter;
