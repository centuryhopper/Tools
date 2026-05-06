import os
import tempfile
import pandas as pd
import streamlit as st
import plotly.express as px

from pyspark.sql import DataFrame, SparkSession
from pyspark.sql.functions import (
    col, input_file_name, regexp_extract, upper, when,
    abs as spark_abs, sum as spark_sum,
    to_date, date_format, round as spark_round, coalesce, try_to_date
)

# -------------------------
# ⚙️ Spark Setup
# -------------------------
os.environ["JAVA_HOME"] = "/usr/lib/jvm/java-17-openjdk-amd64"

@st.cache_resource
def get_spark():
    return SparkSession.builder.appName("BankTransactions").getOrCreate()


# -------------------------
# 📥 File Handling
# -------------------------
def load_uploaded_file(uploaded_file, spark):
    with tempfile.NamedTemporaryFile(delete=False, suffix=".csv") as tmp:
        tmp.write(uploaded_file.getvalue())
        tmp_path = tmp.name

    return spark.read.option("header", True).csv(tmp_path)


# -------------------------
# 🧹 Data Cleaning
# -------------------------
def normalize_columns(df):
    return (
        df.withColumnRenamed("Description", "Name")
          .withColumnRenamed("Transaction Amt", "Amount")
          .withColumnRenamed("TransactionDate", "Date")
          .withColumnRenamed("Post Date", "Date")
          .drop("Trans. Date")
    )


def process_by_source(df, source):
    match source:
        case "fidelity":
            df = df.drop("Memo")
            df = df.filter(col("Transaction") == "DEBIT").drop("Transaction")
            df = df.withColumn("Date", to_date(col("Date")))
            df = df.withColumn("Amount", spark_round(col("Amount").cast("double"), 2))
        case 'discover':
            df = df.withColumn("Date", to_date(col("Date"), "MM/dd/yyyy"))
            df = df.withColumn("Amount", spark_round(col("Amount").cast("double"), 2)) \
               .filter(col("Amount") > 0)

    return df.withColumn("Amount", spark_abs(col("Amount")))


# -------------------------
# 🏦 Metadata
# -------------------------
def add_bank_column(df):
    return (
        df.withColumn("file_path", input_file_name())
          .withColumn("Bank", regexp_extract(col("file_path"), r"/([^/]+)\.csv$", 1))
          .drop("file_path")
    )


# -------------------------
# 🏷️ Categorization
# -------------------------
def build_category_expression(categories):
    category_expr = None

    for category, keywords in categories.items():
        condition = None

        for keyword in keywords:
            keyword_cond = upper(col("Name")).contains(keyword)
            condition = keyword_cond if condition is None else (condition | keyword_cond)

        if category_expr is None:
            category_expr = when(condition, category)
        else:
            category_expr = category_expr.when(condition, category)

    return category_expr.otherwise("Miscellaneous")


def apply_categories(df, categories):
    category_expr = build_category_expression(categories)
    return df.withColumn("Category", category_expr)


# -------------------------
# 📊 Aggregation
# -------------------------
def aggregate_data(df):
    df = df.withColumn("Month", date_format(col("Date"), "yyyy-MM"))

    return (
        df.groupBy("Category", "Month")
          .agg(spark_sum("Amount").alias("TotalAmount"))
          .orderBy("Month")
    )


# -------------------------
# 📈 Visualization
# -------------------------
def plot_data(pdf):
    pdf["Month"] = pdf["Month"].astype(str)
    pdf = pdf.sort_values("Month", key=lambda x: pd.to_datetime(x, format="%Y-%m"))
    fig = px.bar(
        pdf,
        x="Month",
        y="TotalAmount",
        color="Category",
        barmode="stack",
        title="Monthly Spending by Category"
    )
    
    fig.update_xaxes(type="category")  # 🔥 THIS is what fixes Jan 1 issue
    
    st.plotly_chart(fig, use_container_width=True)

def validate_file(uploaded_file, df: DataFrame):
    if not uploaded_file.name.lower().endswith(".csv"):
        st.error("❌ Invalid file type. Please upload a CSV file.")
        st.stop()

    # required_columns = {"Name", "Date", "Amount"}

    # if not required_columns.issubset(set(df.columns)):
    #     st.error(
    #         f"""
    #             ❌ Missing required columns.
    #             Required: {required_columns}
    #             Found: {set(df.columns)}
    #         """
    #     )
    #     st.stop()
        
    # 3. Date validation (supports multiple formats)
    # date_test_df = df.withColumn(
    #     "ParsedDate",
    #     coalesce(
    #         try_to_date(col("Date"), "yyyy-MM-dd"),
    #         try_to_date(col("Date"), "MM/dd/yyyy")
    #     )
    # )

    # invalid_dates = date_test_df.filter(
    #     col("ParsedDate").isNull() & col("Date").isNotNull()
    # )
    
    # if invalid_dates.limit(1).count() > 0:
    #     sample = invalid_dates.select("Date").limit(5).toPandas()
    #     st.error("❌ Invalid date values found.")
    #     st.dataframe(sample)
    #     st.stop()

    # # 4. Amount validation
    # amount_test_df = df.withColumn(
    #     "ParsedAmount",
    #     col("Amount").cast("double")
    # )

    # invalid_amounts = amount_test_df.filter(
    #     col("ParsedAmount").isNull() & col("Amount").isNotNull()
    # )

    # if invalid_amounts.limit(1).count() > 0:
    #     sample = invalid_amounts.select("Amount").limit(5).toPandas()
    #     st.error("❌ Invalid amount values found.")
    #     st.dataframe(sample)
    #     st.stop()

    # st.success("✅ File validated successfully!")

def cast_columns(df):
    df = df.withColumn(
        "Date",
        coalesce(
            try_to_date(col("Date"), "yyyy-MM-dd"),
            try_to_date(col("Date"), "MM/dd/yyyy")
        )
    )
    df = df.withColumn("Amount", col("Amount").cast("double"))
    
     # 🔥 drop bad rows HERE (not earlier)
    df = df.filter(col("Date").isNotNull())
    df = df.filter(col("Amount").isNotNull())
    return df

# -------------------------
# 🚀 Main App
# -------------------------
def main():
    st.title("Transaction Analyzer")
    spark = get_spark()
    uploaded_file = st.file_uploader("Upload transactions CSV", type=["csv"], accept_multiple_files=False)

    if not uploaded_file:
        st.info("Please upload a CSV file to continue.")
        st.stop()
        return

    df = load_uploaded_file(uploaded_file, spark)
    validate_file(uploaded_file, df)
    
    
    source = st.selectbox("Select file type", ["fidelity", "discover"])
    
    df = cast_columns(df)

    df = normalize_columns(df)
    df = process_by_source(df, source)
    df = add_bank_column(df)

    categories = {
        "Income": ["PAYROLL", "DIRECT DEP", "VENMO", "REFUND", ],
        "Housing": ["RENT", "MORTGAGE", "ELECTRIC", "WATER", "GAS", ],
        "Transportation & Gas": ["SHELL", "EXXON", "UBER", "LYFT", "7-ELEVEN", "Sam's Club Gas", "SAMS FUEL", ],
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
    pdf = df.toPandas()
    
    st.dataframe(pdf)
    plot_data(pdf)


'''
    1. Load
    2. Validate (non-mutating)
    3. CLEAN (mutate + fix types)
    4. Drop invalid rows (or handle)
    5. Transform (Month, Category)
    6. Aggregate
    7. Visualize
'''
if __name__ == "__main__":
    main()