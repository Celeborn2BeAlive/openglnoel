/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

const React = require('react');

const CompLibrary = require('../../core/CompLibrary.js');

const MarkdownBlock = CompLibrary.MarkdownBlock; /* Used to read markdown */
const Container = CompLibrary.Container;
const GridBlock = CompLibrary.GridBlock;

class HomeSplash extends React.Component {
    render() {
        const { siteConfig, language = '' } = this.props;
        const { baseUrl, docsUrl } = siteConfig;
        const docsPart = `${docsUrl ? `${docsUrl}/` : ''}`;
        const langPart = `${language ? `${language}/` : ''}`;
        const docUrl = doc => `${baseUrl}${docsPart}${langPart}${doc}`;

        const SplashContainer = props => (
            <div className="homeContainer">
                <div className="homeSplashFade">
                    <div className="wrapper homeWrapper">{props.children}</div>
                </div>
            </div>
        );

        const Logo = props => (
            <div className="projectLogo">
                <img src={props.img_src} alt="Project Logo" />
            </div>
        );

        const ProjectTitle = () => (
            <h2 className="projectTitle">
                {siteConfig.title}
                <small>{siteConfig.tagline}</small>
            </h2>
        );

        const PromoSection = props => (
            <div className="section promoSection">
                <div className="promoRow">
                    <div className="pluginRowBlock">{props.children}</div>
                </div>
            </div>
        );

        const Button = props => (
            <div className="pluginWrapper buttonWrapper">
                <a className="button" href={props.href} target={props.target}>
                    {props.children}
                </a>
            </div>
        );

        return (
            <SplashContainer>
                <Logo img_src={`${baseUrl}img/logo.png`} />
                <div className="inner">
                    <ProjectTitle siteConfig={siteConfig} />
                    <PromoSection>
                        <Button href={docUrl('intro-sdk-01-philogl-01-introduction.html')}>Getting Started</Button>
                        <Button href={docUrl('bronze-02-hello-triangle-01-code-de-base.html')}>Imac 2 / Master 2</Button>
                        <Button href={docUrl('gold-01-intro-sdk-01-introduction.html')}>Imac 3 / Master 2</Button>
                    </PromoSection>
                </div>
            </SplashContainer>
        );
    }
}

class Index extends React.Component {
    render() {
        const { config: siteConfig, language = '' } = this.props;
        const { baseUrl } = siteConfig;

        const Block = props => (
            <Container
                padding={['bottom', 'top']}
                id={props.id}
                background={props.background}>
                <GridBlock
                    align="center"
                    contents={props.children}
                    layout={props.layout}
                />
            </Container>
        );

        const BronzeLevel = () => (
            <Block background="white">
                {[
                    {
                        content: `Apprenez à dessiner en 2D avec OpenGL 3+. Au programme: VBOs, VAOs, Shaders, Textures et Transformations 2D.<hr>Niveau Imac 2 et Master 2.`,
                        image: `${baseUrl}img/iknowopengl-horizontal.png`,
                        imageAlign: 'left',
                        title: 'Bronze Level',
                    },
                ]}
            </Block>
        );

        const SilverLevel = () => (
            <Block background="light">
                {[
                    {
                        content: `Apprenez à dessiner en 3D avec OpenGL 3+. Au programme: Théorie, Transformations 3D, Caméras, Lumières et Matériaux.<hr>Niveau Imac 2 et Master 2.`,
                        image: `${baseUrl}img/dontneedmathgirl-horizontal.png`,
                        imageAlign: 'right',
                        title: 'Silver Level',
                    },
                ]}
            </Block>
        );

        const GoldLevel = () => (
            <Block background="white">
                {[
                    {
                        content: `Techniques avancées pour la 3D avec OpenGL 3+. Au programme: Deferred Rendering, Shadow Mapping, Post Processing, Computes Shaders.<hr>Niveau Imac 3 et Master 2.`,
                        image: `${baseUrl}img/onedoesnotsimplyuseopengl3.jpg`,
                        imageAlign: 'left',
                        title: 'Gold Level',
                    },
                ]}
            </Block>
        );

        const Features = () => (
            <Block layout="fourColumn" background="light">
                {[
                    {
                        content: 'Installation du SDK des TDs pour commencer à coder.',
                        // image: `${baseUrl}img/logo.png`,
                        imageAlign: 'top',
                        title: 'Introduction et SDK',
                    },
                    {
                        content: "OpenGL 3+ pour la 2D.",
                        // image: `${baseUrl}img/logo.png`,
                        imageAlign: 'top',
                        title: 'Bronze Level',
                    },
                    {
                        content: 'OpenGL 3+ pour la 3D.',
                        // image: `${baseUrl}img/logo.png`,
                        imageAlign: 'top',
                        title: 'Silver Level',
                    },
                    {
                        content: 'Techniques avancées en OpenGL pour la 3D.',
                        // image: `${baseUrl}img/logo.png`,
                        imageAlign: 'top',
                        title: 'Gold Level',
                    }
                ]}
            </Block>
        );

        return (
            <div>
                <HomeSplash siteConfig={siteConfig} language={language} />
                <div>
                    <Features />
                    <BronzeLevel />
                    <SilverLevel />
                    <GoldLevel />
                </div>
            </div>
        );
    }
}

module.exports = Index;
