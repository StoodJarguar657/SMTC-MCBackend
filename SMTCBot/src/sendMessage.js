import serverManager from "./serverManager.js"

function parseDiscordMarkdownToMinecraftJson(text) {
    const components = [];

    // First parse markdown links and styles with regex as before
    const regex = /(\[([^\]]+)\]\((https?:\/\/[^\s)]+)\))|(\*\*(.+?)\*\*)|(\*(.+?)\*)|(__([^_]+)__)|(~~(.+?)~~)|(`(.+?)`)/g;

    let lastIndex = 0;
    let match;
    while ((match = regex.exec(text)) !== null) {
        if (match.index > lastIndex) {
            // Process plain text between matches to detect raw URLs
            components.push(...splitTextByRawUrls(text.substring(lastIndex, match.index)));
        }

        if (match[1]) {
            // Markdown link
            components.push({
                text: match[2],
                color: "aqua",
                underlined: true,
                clickEvent: { action: "open_url", value: match[3] },
                hoverEvent: {
                    action: "show_text",
                    contents: [{ text: `Click to open ${match[3]}`, color: "yellow" }]
                }
            });
        } else if (match[4]) {
            components.push({ text: match[5], bold: true, color: "white" });
        } else if (match[6]) {
            components.push({ text: match[7], italic: true, color: "white" });
        } else if (match[8]) {
            components.push({ text: match[9], underlined: true, color: "white" });
        } else if (match[10]) {
            components.push({ text: match[11], strikethrough: true, color: "white" });
        } else if (match[12]) {
            components.push({ text: match[13], obfuscated: true, color: "gray" });
        }

        lastIndex = regex.lastIndex;
    }

    if (lastIndex < text.length) {
        components.push(...splitTextByRawUrls(text.substring(lastIndex)));
    }

    return components;
}

// Helper to split text by raw URLs and wrap URLs as clickable components
function splitTextByRawUrls(text) {
    const urlRegex = /(https?:\/\/[^\s]+)/g;
    const parts = [];
    let lastIndex = 0;
    let match;
    while ((match = urlRegex.exec(text)) !== null) {
        if (match.index > lastIndex) {
            parts.push({ text: text.substring(lastIndex, match.index), color: "white" });
        }

        const url = match[0];
        parts.push({
            text: url,
            color: "aqua",
            underlined: true,
            clickEvent: { action: "open_url", value: url },
            hoverEvent: {
                action: "show_text",
                contents: [{ text: `Click to open ${url}`, color: "yellow" }]
            }
        });
        lastIndex = urlRegex.lastIndex;
    }

    if (lastIndex < text.length) {
        parts.push({ text: text.substring(lastIndex), color: "white" });
    }
    return parts;
}

export default {
    async sendMessage(serverInfo, username, userid, message) {
        const data = [
            { "text": "[" },
            { "text": "Discord", "bold": true, "color": "red" },
            { "text": "] " },
            {
                "text": username,
                "underlined": true,
                "color": "blue",
                "clickEvent": {
                    "action": "open_url",
                    "value": `https://discord.com/users/${userid}`
                },
                "hoverEvent": {
                    "action": "show_text",
                    "contents": "Click to view the §9§lDiscord user§r who sent this message."
                }
            },
            { "text": ": ", "color": "white" },
            ...parseDiscordMarkdownToMinecraftJson(message)
        ];

        return await serverManager.sendRcon(serverInfo, `tellraw @a ${JSON.stringify(data)}`);
    },

    async adminSendMessage(serverInfo, message) {
        const data = [
            { "text": "[" },
            { "text": "Discord", "bold": true, "color": "red" },
            { "text": "] " },
            { "text": "Admin", "bold": true, "color": "dark_red" },
            { "text": ": ", "color": "white" },
            ...parseDiscordMarkdownToMinecraftJson(message)
        ];

        return await serverManager.sendRcon(serverInfo, `tellraw @a ${JSON.stringify(data)}`);
    }
}
