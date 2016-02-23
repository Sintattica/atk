module.exports = function (grunt) {

    grunt.initConfig({
        sass: {
            dist: {
                options: {
                    style: 'expanded'
                },
                files: {
                    '.tmp/style.css': ['sass/style.scss']
                }
            }
        },
        concat: {
            dist: {
                src: ['.tmp/style.css', 'bower_components/smartmenus/src/addons/bootstrap/jquery.smartmenus.bootstrap.css'],
                dest: '../public/styles/style.css'
            }
        },
        uglify: {
            dist: {
                options: {
                    beautify: false
                },
                files: {
                    '../public/javascript/atk.min.js': [
                        'bower_components/bootstrap-sass/assets/javascripts/bootstrap.js',
                        'bower_components/smartmenus/src/jquery.smartmenus.js',
                        'bower_components/smartmenus/src/addons/bootstrap/jquery.smartmenus.bootstrap.js'
                    ]
                }
            }
        },
        copy: {
            dist: {
                files: [
                    {
                        expand: true,
                        cwd: 'bower_components/jquery/dist/',
                        src: ['**'],
                        dest: '../public/javascript/jquery/'
                    },
                    {
                        expand: true,
                        cwd: 'bower_components/bootstrap-sass/assets/fonts/',
                        src: ['**'],
                        dest: '../public/fonts/'
                    },
                    {
                        expand: true,
                        cwd: '.tmp/',
                        src: ['style.css.map'],
                        dest: '../public/styles/'
                    }
                ]
            }
        }
    });

    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-contrib-sass');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-concat');

    grunt.registerTask('default', ['copy', 'sass', 'uglify']);

    grunt.registerTask('css', ['sass', 'concat']);
};