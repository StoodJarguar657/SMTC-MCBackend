import { ChannelType, MessageFlags, SlashCommandBuilder } from "discord.js"
import serverManager from "../src/serverManager.js"

export default {
    data: new SlashCommandBuilder()
        .setName("invitevc")
        .setDescription("Invites all users in the mc server to vc on the discord server.")
        .addChannelOption(option => option.setName("channel").setDescription("The channel to request").setRequired(true).addChannelTypes(ChannelType.GuildVoice)),

    permissionLevel: 1,

    async init() { },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const voiceChannel = interaction.options.getChannel("channel")
        const url = `https://discord.com/channels/${interaction.guildId}/${voiceChannel.id}`

        const username = interaction.user.displayName || interaction.user.username

        const data = [
            { "text": "[" },
            { "text": "SMTC", "bold": true, "color": "red" },
            { "text": "] " },
            {
                "text": username,
                "underlined": true,
                "color": "blue",
                "clickEvent": {
                    "action": "open_url",
                    "value": `https://discord.com/users/${interaction.user.id}`
                },
                "hoverEvent": {
                    "action": "show_text",
                    "contents": "Click to view the §9§lDiscord user§r who sent this message."
                }
            },
            { "text": " has requested to voice chat in " },
            {
                "text": voiceChannel.name,
                "underlined": true,
                "color": "blue",
                "clickEvent": {
                    "action": "open_url",
                    "value": url
                },
                "hoverEvent": {
                    "action": "show_text",
                    "contents": `§e${url}`
                }
            },
            { "text": "!", "color": "white" }
        ]

        const response = await serverManager.sendRcon(serverInfo, `tellraw @a ${JSON.stringify(data)}`)
        if (response.status === "success") {
            return await interaction.reply({
                content: `<@${interaction.member.user.id}> has requested to vc in <#${voiceChannel.id}> with people inside the minecraft server!`
            })
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}