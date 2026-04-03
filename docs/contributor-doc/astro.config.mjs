// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// GitHub Pages project URL: https://<owner>.github.io/csarian/
// CI sets SITE_URL; override BASE_PATH if the repo is ever renamed.
const site = process.env.SITE_URL ?? 'https://tofen5061.github.io';
const base = process.env.BASE_PATH ?? '/csarian';

// https://astro.build/config
export default defineConfig({
	site,
	base,
	integrations: [
		starlight({
			title: 'Sarian Docs',
			logo: {
				light: './src/assets/SarianLogo.png',
				dark: './src/assets/SarianLogo.png',
			},
			sidebar: [
				{
					label: 'Guides',
					items: [
						// Each item here is one entry in the navigation menu.
						{ label: 'Contributing Guide', slug: 'guides/contributing_guide' },
					],
				},
				{
					label: 'Reference',
					autogenerate: { directory: 'reference' },
				},
			],
		}),
	],
});
