import dotenv from "dotenv";
import moment from "moment";
import
{
  Configuration,
  CountryCode,
  PlaidApi,
  PlaidEnvironments,
  Products,
  Transaction,
} from "plaid";

dotenv.config();

class PlaidApiService
{
  private client: PlaidApi;

  constructor()
  {
    const env =
      process.env.PLAID_ENV === "production"
        ? PlaidEnvironments.production
        : PlaidEnvironments.sandbox;

    const config = new Configuration({
      basePath: env,
      baseOptions: {
        headers: {
          "PLAID-CLIENT-ID": process.env.PLAID_CLIENT_ID!,
          "PLAID-SECRET":
            process.env.PLAID_ENV === "production"
              ? process.env.PLAID_PRODUCTION_SECRET!
              : process.env.PLAID_SANDBOX_SECRET!,
        },
      },
    });

    this.client = new PlaidApi(config);
  }

  async pollTransactions(accessToken: string, maxRetries = 5)
  {
    let retries = 0;
    let delayMs = 2000;
    let cursor: string | undefined = undefined;
    let transactions: Transaction[] = [];

    const delay = (ms: number) =>
      new Promise((resolve) => setTimeout(resolve, ms));

    let hasMore = true;
    while (hasMore && retries < maxRetries)
    {
      try
      {
        const response = await this.client.transactionsSync({
          access_token: accessToken,
          cursor,
          count: 100,
          options: {
            include_original_description: true,
          },
        });

        const { added, next_cursor, has_more } = response.data;


        transactions.push(...added);
        cursor = next_cursor;
        hasMore = has_more;

      } catch (err: any)
      {
        const errorCode = err?.response?.data?.error_code;
        if (errorCode === "PRODUCT_NOT_READY")
        {
          console.log(
            `Attempt ${retries + 1}: Product not ready. Retrying in ${delayMs / 1000
            }s...`
          );
          await delay(delayMs);
          delayMs *= 2; // exponential backoff
          retries++;
        } else
        {
          throw err; // some other error
        }
      }
    }

    if (retries > maxRetries)
    {
      throw new Error("Exceeded max retries. Transactions still not ready.");
    }

    return { transactions, cursor }
  }

  async getPast1YearsTransactions(accessToken: string)
  {
    const start_date = moment().subtract(1, "year").format("YYYY-MM-DD");
    const end_date = moment().format("YYYY-MM-DD");
    try
    {
      const response = await this.client.transactionsGet({
        access_token: accessToken,
        start_date,
        end_date,
      });

      const transactions = response.data.transactions;
      console.log(
        `You have ${transactions.length} transactions from ${start_date} to ${end_date}.`
      );

      return transactions;
    } catch (err: any)
    {
      if (err.response?.data)
      {
        console.error("Plaid API error:", err.response.data);
      } else
      {
        console.error("Unknown error occurred:", err);
      }
      throw err; // rethrow so caller can handle it
    }
  }

  async getTransactions(accessToken: string)
  {
    let allTransactions: any[] = [];
    let cursor: string | undefined = undefined;
    let hasMore = true;

    // const accountsResponse = await this.client.accountsGet({
    //   access_token: accessToken,
    // });
    // console.log(
    //   "accountsResponse.data.accounts",
    //   accountsResponse.data.accounts
    // );

    while (hasMore)
    {
      const response = await this.client.transactionsSync({
        access_token: accessToken,
        cursor, // will be null on first call
      });

      const { accounts, next_cursor, has_more } = response.data;

      allTransactions.push(...accounts);
      cursor = next_cursor;
      hasMore = has_more;
    }

    return allTransactions;
  }

  async createLinkToken(userId: string)
  {
    try
    {
      const response = await this.client.linkTokenCreate({
        user: { client_user_id: userId },
        client_name: "Finance App",
        products: [Products.Auth, Products.Transactions],
        country_codes: [CountryCode.Us],
        language: "en",
        redirect_uri: undefined, // optional
      });

      return response.data;
    } catch (err: any)
    {
      throw new Error(
        `Failed to create link token: ${err.response?.data?.error_message || err.message
        }`
      );
    }
  }

  async exchangePublicToken(publicToken: string)
  {
    try
    {
      const response = await this.client.itemPublicTokenExchange({
        public_token: publicToken,
      });
      return response.data;
    } catch (err: any)
    {
      throw new Error(
        `Failed to exchange public token: ${err.response?.data?.error_message || err.message
        }`
      );
    }
  }

  async getAuthInfo(accessToken: string)
  {
    try
    {
      const response = await this.client.authGet({ access_token: accessToken });
      return response.data;
    } catch (err: any)
    {
      throw new Error(
        `Failed to retrieve auth info: ${err.response?.data?.error_message || err.message
        }`
      );
    }
  }
}

export default PlaidApiService;
