module.exports = function (grunt) {

    grunt.initConfig({
        copy: {
            dist: {
                files: [
                    {
                        expand: true,
                        cwd: 'bower_components/bootstrap-sass/assets/fonts/',
                        src: ['**'],
                        dest: '../public/fonts/'
                    },
                    {
                        expand: true,
                        cwd: 'bower_components/font-awesome-sass/assets/fonts/',
                        src: ['**'],
                        dest: '../public/fonts/'
                    }
                ]
            }
        },
        sass: {
            dist: {
                options: {
                    sourceMap: true
                },
                files: {
                    '../public/styles/style.css': 'sass/style.scss'
                }
            }
        },
        uglify: {
            dist: {
                options: {
                    beautify: false,
                    preserveComments: 'some'
                },
                files: {
                    '../public/javascript/libs.min.js': [
                        'bower_components/jquery/dist/jquery.js',
                        'bower_components/bootstrap-sass/assets/javascripts/bootstrap.js',
                        'bower_components/smartmenus/src/jquery.smartmenus.js',
                        'bower_components/smartmenus/src/addons/bootstrap/jquery.smartmenus.bootstrap.js',
                        'bower_components/select2/dist/js/select2.full.js',
                        'bower_components/select2/dist/js/i18n/*.js'
                    ]
                }
            }
        },
        watch: {
            css: {
                files: ['sass/*.scss'],
                tasks: ['sass'],
                options: {
                    livereload: true
                }
            }
        }
    });

    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-sass');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-watch');

    grunt.registerTask('default', ['copy', 'sass', 'uglify']);
};