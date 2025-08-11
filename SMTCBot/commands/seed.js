import { MessageFlags, SlashCommandBuilder } from "discord.js"
import serverManager from "../src/serverManager.js"

export default {
    data: new SlashCommandBuilder()
        .setName("seed")
        .setDescription("Gets the current seed of the world"),

    permissionLevel: 1,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const response = await serverManager.sendRcon(serverInfo, "seed")
        if(response.status !== "success")
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
        
        await interaction.reply({ content: `**Server Seed:** \`${response.message.slice(7, -1)}\``, flags: MessageFlags.Ephemeral})
    }
}