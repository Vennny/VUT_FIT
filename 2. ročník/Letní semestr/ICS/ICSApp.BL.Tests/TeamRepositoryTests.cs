using System;
using System.Collections.Generic;
using System.Text;
using ICSapp.BL.Models;
using Xunit;

namespace ICSapp.BL.Tests
{
    public class TeamRepositoryTests : IClassFixture<TeamRepositoryTestsFixture>
   {
        private readonly TeamRepositoryTestsFixture fixture;

        public TeamRepositoryTests(TeamRepositoryTestsFixture fixture)
        {
            this.fixture = fixture;
        }
        [Fact]
        public void GetByID_Test()
        {
            var teamModel = new TeamModel
            {
                TeamName = "Bondovka"
            };

            var returnedTeamModel = fixture.UserRepository.CreateTeam(teamModel);
            var returnedByID = fixture.Repository.GetById(returnedTeamModel.Id);

            Assert.Equal(returnedTeamModel.Id,returnedByID.Id);
            
            fixture.Repository.RemoveTeam(returnedTeamModel.Id);
        }

        [Fact]
        public void AddTeamMate_Test()
        {
            var userModel1 = new UserModel
            {
                EmailAdress = "DavidValecky2@centrum.cz",
                Password = "rabat587",
                FirstName = "Otakar",
                Surname = "Březina",
                Description = "170cm blonďák, tetování až na patách.",
                Activity = false,
                LastActivity = DateTime.Now
            };

            var teamModel = new TeamModel
            {
                TeamName = "Gargy team"
            };

            var returnedUserModel1 = fixture.UserRepository.CreateUser(userModel1);

            var returnedTeamModel = fixture.UserRepository.CreateTeam(teamModel);

            var returnedTeamModelAfter = fixture.Repository.AddTeamMate(returnedTeamModel.Id, returnedUserModel1.Id);
            returnedTeamModelAfter = fixture.Repository.AddTeamMate(returnedTeamModel.Id, returnedUserModel1.Id);
            var returnedById = fixture.Repository.GetById(returnedTeamModelAfter.Id);
           
            Assert.NotNull(returnedTeamModelAfter);

            fixture.Repository.RemoveTeam(returnedTeamModelAfter.Id);
            fixture.UserRepository.RemoveUser(returnedUserModel1.Id);
        }

        //[Fact]
        public void RemoveTeamMate_Test()
        {            
            var userModel = new UserModel
            {
                EmailAdress = "DavidValecky@centrum.cz",
                Password = "rabat587",
                FirstName = "David",
                Surname = "Valecký",
                Description = "170cm blonďák, tetování až na patách.",
                Activity = false,
                LastActivity = DateTime.Now
            };

            var teamModel = new TeamModel
            {
                TeamName = "Bondovka"
            };

            var returnedTeamModel = fixture.UserRepository.CreateTeam(teamModel);
            var returnedUserModel = fixture.UserRepository.CreateUser(userModel);

            fixture.Repository.AddTeamMate(returnedTeamModel.Id, returnedUserModel.Id);
            var returnedTeamWithMember = fixture.Repository.GetById(returnedTeamModel.Id);

            fixture.Repository.RemoveTeamMate(returnedTeamWithMember.Id, returnedUserModel.Id);
            var returnedTeamWithoutMemeber = fixture.Repository.GetById(returnedTeamWithMember.Id);
            
            Assert.Equal(returnedTeamModel, returnedTeamWithoutMemeber);

            fixture.Repository.RemoveTeam(returnedTeamModel.Id);
            fixture.UserRepository.RemoveUser(userModel.Id);
        }

        //[Fact]
        public void AddPost_Test()
        {
            var userModel1 = new UserModel
            {
                EmailAdress = "DavidValecky@centrum.cz",
                Password = "rabat587",
                FirstName = "David",
                Surname = "Valecký",
                Description = "170cm blonďák, tetování až na patách.",
                Activity = false,
                LastActivity = DateTime.Now
            };

            var postModel = new PostModel
            {
                Author = userModel1,
                MessageContent = "Já som hroch",
                Time = DateTime.Now
            };

            var teamModel1 = new TeamModel
            {
                TeamName = "Bondovka"
            };

            var teamModel2 = new TeamModel
            {
                TeamName = "Gargy team",
                Posts = new List<PostModel>
                {
                    new PostModel
                    {
                        Author = userModel1,
                        MessageContent = "Já som hroch",
                        Time = DateTime.Now
                    }
                }
            };

            var returnedTeamModel1 = fixture.UserRepository.CreateTeam(teamModel1);
            var returnedTeamModel2 = fixture.UserRepository.CreateTeam(teamModel2);
            var returnedUserModel = fixture.UserRepository.CreateUser(userModel1);
            var returnedPostModel = fixture.Repository.CreatePost(postModel, returnedUserModel.Id);
            var returnedTeamModelWithPost = fixture.Repository.AddPost(returnedTeamModel1.Id, returnedPostModel.Id);
            Assert.NotEmpty(returnedTeamModelWithPost.Posts);
            fixture.Repository.RemoveTeam(returnedTeamModel1.Id);
            fixture.Repository.RemoveTeam(returnedTeamModel2.Id);
        }

       [Fact]
        public void RemoveTeam_Test()
        {
            var teamModel = new TeamModel
            {
                TeamName = "Bondovka"
            };

            var returnedTeamModel1 = fixture.UserRepository.CreateTeam(teamModel);
            fixture.Repository.RemoveTeam(returnedTeamModel1.Id);

            Assert.Null(fixture.Repository.GetById(returnedTeamModel1.Id));
            
        }
    }
}
