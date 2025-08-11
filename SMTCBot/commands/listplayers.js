import { AttachmentBuilder, MessageFlags, SlashCommandBuilder } from "discord.js";
import serverManager from "../src/serverManager.js";
import Table from "cli-table3";

export default {
    data: new SlashCommandBuilder()
        .setName("list-players")
        .setDescription("Lists all players"),

    permissionLevel: 1,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const response = await serverManager.sendRcon(serverInfo, "list uuids");
        if (response.status !== "success") {
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral });
        }

        const message = `${response.message}`.split("online: ")[1];
        const users = message.split(", ");

        const table = new Table({
            head: ["Username", "Player Uuid", "NameMc Link"],
            colWidths: [20, 40, 50],
            wordWrap: true,
            style: {
                head: [],
                border: []
            }
        });

        for (const userUuid of users) {
            const match = userUuid.match(/^(.+?) \(([\w-]{36})\)$/);
            if (!match) continue;

            const [_, username, uuid] = match;
            const namemc = `https://namemc.com/profile/${username}`;
            table.push([username, uuid, namemc]);
        }

        const tableOutput = table.toString();
        if (tableOutput.length <= (2000 - 8)) {
            return await interaction.reply({ content: `\`\`\`\n${tableOutput}\n\`\`\``, flags: MessageFlags.Ephemeral });
        }

        const buffer = Buffer.from(tableOutput, "utf-8");
        const attachment = new AttachmentBuilder(buffer, { name: "response.txt" });
        await interaction.reply({ files: [attachment], flags: MessageFlags.Ephemeral });
    }
};
