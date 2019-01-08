/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// See https://docusaurus.io/docs/site-config for all the possible
// site configuration options.

// List of projects/orgs using your project for the users page.
const users = [
    // {
    //     caption: 'User1',
    //     // You will need to prepend the image path with your baseUrl
    //     // if it is not '/', like: '/test-site/img/docusaurus.svg'.
    //     image: '/img/logo.png',
    //     infoLink: 'https://www.facebook.com',
    //     pinned: true,
    // },
];

const siteConfig = {
    title: 'OpenGL["noël"]', // Title for your website.
    tagline: "TDs de l'Université Paris Est Marne-la-Vallée",
    url: 'https://celeborn2bealive.github.io', // Your website URL
    baseUrl: '/openglnoel/', // Base URL for your project */
    // For github.io type URLs, you would set the url and baseUrl like:
    //   url: 'https://facebook.github.io',
    //   baseUrl: '/test-site/',

    // Used for publishing and more
    projectName: 'openglnoel',
    organizationName: 'celeborn2bealive',
    // For top-level user or org sites, the organization is still the same.
    // e.g., for the https://JoelMarcey.github.io site, it would be set like...
    //   organizationName: 'JoelMarcey'

    // For no header links in the top nav bar -> headerLinks: [],
    headerLinks: [
        { doc: 'intro-sdk-todo', label: 'Introduction et SDK' },
        { doc: 'bronze-01-philogl-01-introduction', label: 'Bronze Level' },
        { doc: 'silver-01-intro-sdk-01-introduction', label: 'Silver Level' },
        { blog: true, label: 'Blog' },
        { href: 'http://docs.gl', label: 'OpenGL Documentation' },
        { href: 'https://www.khronos.org/opengl/wiki/', label: 'OpenGL Wiki' },
        { href: 'https://github.com/celeborn2bealive/openglnoel', label: 'GitHub' }
    ],

    // If you have users set above, you add it here:
    users,

    /* path to images for header/footer */
    headerIcon: 'img/logo.png',
    footerIcon: 'img/logo.png',
    favicon: 'img/favicon.png',

    /* Colors for website */
    colors: {
        primaryColor: '#0D3E5C',
        secondaryColor: '#061f2e',
    },

    /* Custom fonts for website */
    /*
    fonts: {
      myFont: [
        "Times New Roman",
        "Serif"
      ],
      myOtherFont: [
        "-apple-system",
        "system-ui"
      ]
    },
    */

    // This copyright info is used in /core/Footer.js and blog RSS/Atom feeds.
    copyright: `Copyright © ${new Date().getFullYear()} Laurent NOEL`,

    highlight: {
        // Highlight.js theme to use for syntax highlighting in code blocks.
        theme: 'atom-one-dark',
    },

    // Add custom scripts here that would be placed in <script> tags.
    scripts: [
        'https://buttons.github.io/buttons.js',
        'https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/latest.js?config=TeX-MML-AM_CHTML',
        'https://cdnjs.cloudflare.com/ajax/libs/clipboard.js/2.0.0/clipboard.min.js',
        '/openglnoel/js/code-block-buttons.js',
        "/openglnoel/js/disqus.js"
    ],

    // On page navigation for the current documentation page.
    onPageNav: 'separate',
    // No .html extensions for paths.
    cleanUrl: true,

    // Open Graph and Twitter card images.
    ogImage: 'img/logo.png',
    twitterImage: 'img/logo.png',

    // Show documentation's last contributor's name.
    // enableUpdateBy: true,

    // Show documentation's last update time.
    // enableUpdateTime: true,

    // You may provide arbitrary config keys to be used as needed by your
    // template. For example, if you need your repo's URL...
    repoUrl: 'https://github.com/celeborn2bealive/openglnoel',
};

module.exports = siteConfig;
