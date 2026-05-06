import pandas as pd
import streamlit as st
import plotly.express as px

# -------------------------
# 📥 File Handling
# -------------------------
def load_uploaded_file(uploaded_file):
    return pd.read_csv(uploaded_file)


# -------------------------
# 🧹 Data Cleaning
# -------------------------



def cast_columns(df):
    df["Date"] = pd.to_datetime(df["Date"], errors="coerce")
    df["Amount"] = pd.to_numeric(df["Amount"], errors="coerce")

    # drop invalid rows
    df = df.dropna(subset=["Date", "Amount"])
    return df

# TODO: include a custom source so that users can set it up to their own liking (add more streamlit prompts)
def process_by_source(df, source):
    if source == "fidelity":
        if "Transaction" in df.columns:
            df = df[df["Transaction"] == "DEBIT"]
           
        # dont need this because those three columns already exist 
        # df = df.rename(columns={
        #     "Date": "Date",
        #     "Name": "Name",
        #     "Amount": "Amount",
        # })
        
        df = df[["Name", "Date", "Amount"]]
        

    elif source == "discover":
        df = df.rename(columns={
            "Description": "Name",
            "Post Date": "Date",
            "Amount": "Amount",
        })
        
        df = df[["Name", "Date", "Amount"]]
        
        df = df[df["Amount"] > 0]
    elif not source:
        return None

    df["Amount"] = df["Amount"].abs().round(2)
    return df


# -------------------------
# 🏷️ Categorization
# -------------------------
def apply_categories(df, categories):
    df["Name"] = df["Name"].astype(str).str.upper()
    df["Category"] = "Miscellaneous"

    for category, keywords in categories.items():
        keywords_upper = [k.upper() for k in keywords]
        mask = df["Name"].str.upper().str.contains("|".join(keywords_upper), na=False)
        df.loc[mask, "Category"] = category

    return df


# -------------------------
# 📊 Aggregation
# -------------------------
def aggregate_data(df):
    df["Month"] = df["Date"].dt.to_period("M").astype(str)

    return (
        df.groupby(["Category", "Month"])["Amount"]
        .sum()
        .reset_index(name="TotalAmount")
        .sort_values("Month")
    )


# -------------------------
# 📈 Visualization
# -------------------------
def plot_data(df):
    df = df.sort_values("Month")

    fig = px.bar(
        df,
        x="Month",
        y="TotalAmount",
        color="Category",
        barmode="stack",
        title="Monthly Spending by Category"
    )

    fig.update_xaxes(type="category")
    st.plotly_chart(fig, width='stretch')


# -------------------------
# 🚀 Main App
# -------------------------
# In terminal, type this to run the app:
#   - poetry run streamlit run transactions_processor_pipeline.py
def main():
    st.title("Transaction Analyzer")

    uploaded_file = st.file_uploader("Upload transactions CSV", type=["csv"])

    if not uploaded_file:
        st.info("Please upload a CSV file to continue.")
        return

    try:
        df = load_uploaded_file(uploaded_file)
    except Exception as e:
        st.error(f"Failed to read file: {e}")
        return

    source = st.selectbox("Select file type", ["", "fidelity", "discover"])

    df = process_by_source(df, source)
    if df is None:
        st.error(f"Please select a file type.")
        return
    df = cast_columns(df)

    # TODO: Ask for a json file so users can tailor it to their own liking
    categories = {
        "Income": ["PAYROLL", "DIRECT DEP", "VENMO", "REFUND", ],
        "Housing": ["RENT", "MORTGAGE", "ELECTRIC", ],
        "Transportation & Gas": ["SHELL", "EXXON", "UBER", "LYFT", "7-ELEVEN", "Sam's Club Gas", "SAMS FUEL", "GAS", ],
        "Food & Dining": ["STARBUCKS", "DOMINOS", "DOORDASH", "TACO", "MCDONALDS", ],
        "Grocery": ["COSTCO", "KROGER", "TRADER JOE", "WALMART", "ALDI", "WM SUPERCENTER", "BRAVO", "WHOLEFDS", "Enson Market Inc", "PUBLIX", "LOTTE PLAZA", "SAMS STORE"],
        "Entertainment": ["NETFLIX", "SPOTIFY", "DISNEY", ],
        "Health & Medical": ["CVS", "WALGREENS", "KAISER", ],
        "Shopping & Personal": ["AMAZON", "BESTBUY", "MACY", ],
        "Travel": ["HOTEL", "AIRBNB", "DELTA", ],
        "Financial / Investments": ["CREDIT", "IRA", "401K", "LOAN", ]
    }

    df = apply_categories(df, categories)
    df = aggregate_data(df)

    st.dataframe(df)
    plot_data(df)


if __name__ == "__main__":
    main()