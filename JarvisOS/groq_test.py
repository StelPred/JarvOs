from groq import Groq

# 🔑 PUT YOUR KEY HERE (no spaces, no quotes issues)
client = Groq(api_key="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")

print("TESTING GROQ...")

response = client.chat.completions.create(
    model="llama-3.1-8b-instant",
    messages=[
        {
            "role": "user",
            "content": "Say hello in one short sentence"
        }
    ]
)

print(response.choices[0].message.content)