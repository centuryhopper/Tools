import crypto from "crypto";

const algorithm = "aes-256-cbc";
const key = crypto
    .createHash("sha256")
    .update(process.env.ENCRYPTION_KEY!) // Must be 32 bytes
    .digest();


export function encrypt(text: string): { iv: string; encryptedData: string }
{
    const iv = crypto.randomBytes(16); // 16 bytes
    const cipher = crypto.createCipheriv(algorithm, key, iv);
    let encrypted = cipher.update(text, "utf8", "hex");
    encrypted += cipher.final("hex");
    return { iv: iv.toString("hex"), encryptedData: encrypted };
}

export function decrypt(ivHex: string, encryptedData: string): string
{
    const iv = Buffer.from(ivHex, "hex");
    const decipher = crypto.createDecipheriv(algorithm, key, iv);
    let decrypted = decipher.update(encryptedData, "hex", "utf8");
    decrypted += decipher.final("utf8");
    return decrypted;
}
